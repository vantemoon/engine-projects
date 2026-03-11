#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct DebugRenderObject
{
	Timer m_timer = Timer( 0.0 );

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
	DebugRenderMode m_mode = DebugRenderMode::USE_DEPTH;

	bool m_isWorld = true;
	bool m_isText = false;
	bool m_isBillboard = false;
	bool m_isMessage = false;
	bool m_isWireframe = false;

	std::vector<Vertex> m_verts;
	Mat44 m_transform;
	Vec3 m_billboardOrigin = Vec3::ZERO;

	std::string m_text;
	float m_textHeight = 0.f;
	Vec2 m_alignment = Vec2::ZERO;
	AABB2 m_screenBox = AABB2::ZERO_TO_ONE;
};


//-----------------------------------------------------------------------------------------------
struct DebugRenderSystem
{
	BitmapFont* m_font = nullptr;
	bool m_isVisible = true;

	std::vector<DebugRenderObject*> m_worldObjects;
	std::vector<DebugRenderObject*> m_screenObjects;
	std::vector<DebugRenderObject*> m_messages;
};


//-----------------------------------------------------------------------------------------------
static DebugRenderSystem g_debugRenderSystem = DebugRenderSystem();


//-----------------------------------------------------------------------------------------------
static Rgba8 GetCurrentColorForObject( DebugRenderObject const* object )
{
	float elapsedFraction = static_cast<float>( object->m_timer.GetElapsedFraction() );
	if ( elapsedFraction < 0.f )
	{
		elapsedFraction = 0.f;
	}
	if ( elapsedFraction > 1.f )
	{
		elapsedFraction = 1.f;
	}

	return Rgba8::WHITE.Interpolate( object->m_startColor, object->m_endColor, elapsedFraction );
}


//-----------------------------------------------------------------------------------------------
static Rgba8 GetXRayFirstPassColor( Rgba8 const& currentColor )
{
	constexpr float lightening = 0.35f;
	constexpr float alphaScale = 0.6f;

	unsigned char lightR = static_cast<unsigned char>( currentColor.r + ( 255 - currentColor.r ) * lightening );
	unsigned char lightG = static_cast<unsigned char>( currentColor.g + ( 255 - currentColor.g ) * lightening );
	unsigned char lightB = static_cast<unsigned char>( currentColor.b + ( 255 - currentColor.b ) * lightening );
	unsigned char lightA = static_cast<unsigned char>( static_cast<float>( currentColor.a ) * alphaScale );

	return Rgba8( lightR, lightG, lightB, lightA );
}


//-----------------------------------------------------------------------------------------------
static void SetVertexColorForObject( DebugRenderObject* object, Rgba8 const& color )
{
	for ( Vertex& vert : object->m_verts )
	{
		vert.m_color = color;
	}
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSystemStartup( DebugRenderConfig const& config )
{
	std::string fontFilePath = config.m_fontPath + config.m_fontName;
	g_debugRenderSystem.m_font = g_engine->m_renderer->CreateOrGetBitmapFontFromFile( fontFilePath.c_str() );

	SubscribeEventCallbackFunction( "DebugRenderClear", Command_DebugRenderClear );
	SubscribeEventCallbackFunction( "DebugRenderToggle", Command_DebugRenderToggle );
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSystemShutdown()
{
	g_debugRenderSystem.m_font = nullptr;
	
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++ index )
	{
		DebugRenderObject* worldObject = g_debugRenderSystem.m_worldObjects[index];
		delete worldObject;
		g_debugRenderSystem.m_worldObjects[index] = nullptr;
	}
	g_debugRenderSystem.m_worldObjects.clear();

	for ( int index = 0; index < g_debugRenderSystem.m_screenObjects.size(); ++index )
	{
		DebugRenderObject* screenObject = g_debugRenderSystem.m_screenObjects[index];
		delete screenObject;
		g_debugRenderSystem.m_screenObjects[index] = nullptr;
	}
	g_debugRenderSystem.m_screenObjects.clear();

	for ( int index = 0; index < g_debugRenderSystem.m_messages.size(); ++index )
	{
		DebugRenderObject* message = g_debugRenderSystem.m_messages[index];
		delete message;
		g_debugRenderSystem.m_messages[index] = nullptr;
	}
	g_debugRenderSystem.m_messages.clear();
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSetVisible()
{
	g_debugRenderSystem.m_isVisible = true;
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSetHidden()
{
	g_debugRenderSystem.m_isVisible = false;
}


//-----------------------------------------------------------------------------------------------
void DebugRenderClear()
{
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		DebugRenderObject* worldObject = g_debugRenderSystem.m_worldObjects[index];
		delete worldObject;
		g_debugRenderSystem.m_worldObjects[index] = nullptr;
	}
	g_debugRenderSystem.m_worldObjects.clear();

	for ( int index = 0; index < g_debugRenderSystem.m_screenObjects.size(); ++index )
	{
		DebugRenderObject* screenObject = g_debugRenderSystem.m_screenObjects[index];
		delete screenObject;
		g_debugRenderSystem.m_screenObjects[index] = nullptr;
	}
	g_debugRenderSystem.m_screenObjects.clear();

	for ( int index = 0; index < g_debugRenderSystem.m_messages.size(); ++index )
	{
		DebugRenderObject* message = g_debugRenderSystem.m_messages[index];
		delete message;
		g_debugRenderSystem.m_messages[index] = nullptr;
	}
	g_debugRenderSystem.m_messages.clear();

}


//-----------------------------------------------------------------------------------------------
void DebugRenderBeginFrame()
{
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		DebugRenderObject* worldObject = g_debugRenderSystem.m_worldObjects[index];
		if ( worldObject != nullptr && worldObject->m_timer.m_period > 0.0 && worldObject->m_timer.HasPeriodElapsed() )
		{
			delete worldObject;
			g_debugRenderSystem.m_worldObjects[index] = nullptr;
		}
	}

	for ( int index = 0; index < g_debugRenderSystem.m_screenObjects.size(); ++index )
	{
		DebugRenderObject* screenObject = g_debugRenderSystem.m_screenObjects[index];
		if ( screenObject != nullptr && screenObject->m_timer.m_period > 0.0 && screenObject->m_timer.HasPeriodElapsed() )
		{
			delete screenObject;
			g_debugRenderSystem.m_screenObjects[index] = nullptr;
		}
	}

	for ( int index = 0; index < g_debugRenderSystem.m_messages.size(); ++index )
	{
		DebugRenderObject* message = g_debugRenderSystem.m_messages[index];
		if ( message != nullptr && message->m_timer.m_period > 0.0 && message->m_timer.HasPeriodElapsed() )
		{
			delete message;
			g_debugRenderSystem.m_messages[index] = nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DebugRenderWorld( Camera const& camera )
{
	if ( !g_debugRenderSystem.m_isVisible )
	{
		return;
	}

	g_engine->m_renderer->BeginCamera( camera );

	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		DebugRenderObject* worldObject = g_debugRenderSystem.m_worldObjects[index];

		if ( worldObject == nullptr )
		{
			continue;
		}

		Rgba8 currentColor = GetCurrentColorForObject( worldObject );

		switch ( worldObject->m_mode )
		{
			case DebugRenderMode::ALWAYS:
			{
				SetVertexColorForObject( worldObject, currentColor );
				g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
				g_engine->m_renderer->SetDepthMode( DepthMode::DISABLED );

				if ( worldObject->m_isWireframe ) g_engine->m_renderer->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_BACK );
				else g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK);

				g_engine->m_renderer->DrawVertexArray( worldObject->m_verts );

				break;
			}
			case DebugRenderMode::X_RAY:
			{
				Rgba8 xrayColor = GetXRayFirstPassColor( currentColor );
				SetVertexColorForObject( worldObject, xrayColor );
				g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
				g_engine->m_renderer->SetDepthMode( DepthMode::READ_ONLY_ALWAYS );

				if ( worldObject->m_isWireframe ) g_engine->m_renderer->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_BACK );
				else g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

				g_engine->m_renderer->DrawVertexArray( worldObject->m_verts );

				SetVertexColorForObject( worldObject, currentColor );
				g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
				g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );

				if ( worldObject->m_isWireframe ) g_engine->m_renderer->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_BACK );
				else g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

				g_engine->m_renderer->DrawVertexArray( worldObject->m_verts );

				break;
			}
			case DebugRenderMode::USE_DEPTH:
			default:
			{
				SetVertexColorForObject( worldObject, currentColor );
				g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
				g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );

				if ( worldObject->m_isWireframe ) g_engine->m_renderer->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_BACK );
				else g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

				g_engine->m_renderer->DrawVertexArray( worldObject->m_verts );

				break;
			}
		}
	}

	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->EndCamera( camera );
}


//-----------------------------------------------------------------------------------------------
void DebugRenderScreen( Camera const& camera )
{
	
}


//-----------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		DebugRenderObject* worldObject = g_debugRenderSystem.m_worldObjects[index];
		if ( worldObject != nullptr && worldObject->m_timer.m_period == 0.0 )
		{
			delete worldObject;
			g_debugRenderSystem.m_worldObjects[index] = nullptr;
		}
	}

	for ( int index = 0; index < g_debugRenderSystem.m_screenObjects.size(); ++index )
	{
		DebugRenderObject* screenObject = g_debugRenderSystem.m_screenObjects[index];
		if ( screenObject != nullptr && screenObject->m_timer.m_period == 0.0 )
		{
			delete screenObject;
			g_debugRenderSystem.m_screenObjects[index] = nullptr;
		}
	}

	for ( int index = 0; index < g_debugRenderSystem.m_messages.size(); ++index )
	{
		DebugRenderObject* message = g_debugRenderSystem.m_messages[index];
		if ( message != nullptr && message->m_timer.m_period == 0.0 )
		{
			delete message;
			g_debugRenderSystem.m_messages[index] = nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldSphere( Vec3 const& center, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = false;

	AddVertsForSphere3D( object->m_verts, center, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireSphere( Vec3 const& center, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = true;

	AddVertsForSphere3D( object->m_verts, center, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldCylinder( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = false;

	AddVertsForCylinder3D( object->m_verts, start, end, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireCylinder( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = true;

	AddVertsForCylinder3D( object->m_verts, start, end, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldArrow( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = false;

	AddVertsForArrow3D( object->m_verts, start, end, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireArrow( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = startColor;
	object->m_endColor = endColor;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = true;

	AddVertsForArrow3D( object->m_verts, start, end, radius, startColor );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddBasis( Mat44 const& transform, float duration, float length, float radius, float colorScale, float alphaScale, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = Rgba8::WHITE;
	object->m_endColor = Rgba8::WHITE;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = false;

	Rgba8 scaledRed = Rgba8( static_cast<unsigned char>( 255 * colorScale ), 0, 0, static_cast<unsigned char>( 255 * alphaScale ) );
	Rgba8 scaledGreen = Rgba8( 0, static_cast<unsigned char>( 255 * colorScale ), 0, static_cast<unsigned char>( 255 * alphaScale ) );
	Rgba8 scaledBlue = Rgba8( 0, 0, static_cast<unsigned char>( 255 * colorScale ), static_cast<unsigned char>( 255 * alphaScale ) );
	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + transform.GetIBasis3D().GetNormalized() * length, radius, duration, scaledRed, scaledRed, mode );
	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + transform.GetJBasis3D().GetNormalized() * length, radius, duration, scaledGreen, scaledGreen, mode );
	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + transform.GetKBasis3D().GetNormalized() * length, radius, duration, scaledBlue, scaledBlue, mode );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldBasis( Mat44 const& transform, float duration, DebugRenderMode mode )
{
	DebugRenderObject* object = new DebugRenderObject();
	object->m_timer = Timer( duration );
	object->m_timer.Start();
	object->m_startColor = Rgba8::WHITE;
	object->m_endColor = Rgba8::WHITE;
	object->m_mode = mode;
	object->m_isWorld = true;
	object->m_isWireframe = false;

	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + Vec3( 1.f, 0.f, 0.f ), 0.08f, duration, Rgba8::RED, Rgba8::RED, mode );
	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + Vec3( 0.f, 1.f, 0.f ), 0.08f, duration, Rgba8::GREEN, Rgba8::GREEN, mode );
	DebugAddWorldArrow( transform.GetTranslation3D(), transform.GetTranslation3D() + Vec3( 0.f, 0.f, 1.f ), 0.08f, duration, Rgba8::BLUE, Rgba8::BLUE, mode );
	for ( int index = 0; index < g_debugRenderSystem.m_worldObjects.size(); ++index )
	{
		if ( g_debugRenderSystem.m_worldObjects[index] == nullptr )
		{
			g_debugRenderSystem.m_worldObjects[index] = object;
			return;
		}
	}
	g_debugRenderSystem.m_worldObjects.push_back( object );
}


//-----------------------------------------------------------------------------------------------
bool Command_DebugRenderClear( EventArgs& args )
{
	UNUSED( args );
	DebugRenderClear();
	return true;
}


//-----------------------------------------------------------------------------------------------
bool Command_DebugRenderToggle( EventArgs& args )
{
	UNUSED( args );
	g_debugRenderSystem.m_isVisible = !g_debugRenderSystem.m_isVisible;
	return true;
}