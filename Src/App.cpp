#include <App.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <fstream>
#include <sstream>

#define SDL_CHECK( res, msg, ... ) { if ( !res ) { printf( msg, __VA_ARGS__ ); return false; } }
#define SDL_CHECK_C( c, msg, ... ) { if ( c < 0 ) { printf( msg, __VA_ARGS__ ); return false; } }

namespace Goi
{

    List<String> LoadTxt(
        const String& path,
		bool& fail
    )
    {
        std::ifstream file( path );
        if ( ! file.is_open() )
        {
            file = std::ifstream( "Assets/" + path );
            
            if ( ! file.is_open() )
            {
                printf( "[IO ERROR] Failed to open file '%s'.", path.c_str() );
                auto ret = LoadTxt( "instructions.txt", fail );
				fail = true;
				return ret;
            }
        }

        List<String> lines;
        String line;
        while ( std::getline( file, line ) )
        {
            lines.push_back( line );
            line.clear();
        }

        file.close();

		fail = false;

        return lines;
    }

    App::App()
        : m_name                {}
        , m_clientSize          {}
        , m_bShouldQuit         {}
        , m_worldSurface        { nullptr }
        , m_worldDrawCountCache {}
        , m_bSimulating         {}
        , m_bLoading            {}
        , m_keyCoolDown         { 0.0f }
        , m_loadInput           { "" }
        , m_bBlockInput         {}
		, m_bPrintFailureMsg	{}
    {
    }

    bool App::Init(
        const String& name,
        const Extent2i& clientSize
    )
    {
        m_name       = name;
        m_clientSize = clientSize;

        return InitSDL() && InitTTF() && InitWorld();
    }

    bool App::Init(
        const String& name,
        const EAspectRatio& aspectRatio,
        const s32& resolution
    )
    {
        Extent2i extent{};
        if ( aspectRatio == k4_3 )
            extent = { resolution / 3 * 4, resolution };
        if ( aspectRatio == k16_9 )
            extent = { resolution / 9 * 16, resolution };

        return Init( name, extent );
    }

    void App::Quit()
    {
        SDL_DestroyRenderer( m_renderer );
        SDL_DestroyWindow( m_window );

        SDL_Quit();
    }

    void App::Run()
    {
        m_timer.Tick();
        f32 frame = m_timer.DeltaSec();
        f32 key   = m_timer.DeltaSec();

        while ( ! m_bShouldQuit )
        {
            if (m_bBlockInput)
                m_bBlockInput = false;

            if ( key > m_keyCoolDown )
            {
                HandleEvents();
                m_keyCoolDown = 0.0f;
                key = 0.0f;
            }

            if ( m_bSimulating &&
                 frame > 1.0f / 20.0f )
            {
                Update();

                frame = 0.0f;
            }

            Render();

            m_timer.Tick();
            frame += m_timer.DeltaSec();
            key   += m_timer.DeltaSec();
        }
    }

    bool App::InitSDL()
    {
        SDL_CHECK_C(
            SDL_Init( SDL_INIT_EVERYTHING ),
            "[SDL ERROR] Failed to init SDL."
        );

        SDL_CHECK(
            ( m_window = SDL_CreateWindow( m_name.c_str(), Exp_Extent2( m_clientSize ), 0u ) ),
            "[SDL ERROR] Failed to create window: %s",
            SDL_GetError()
        );
        SDL_GetWindowSizeInPixels( m_window, &m_clientSize.width, &m_clientSize.height );

        SDL_CHECK(
            ( m_renderer = SDL_CreateRenderer( m_window, nullptr, 0u ) ),
            "[SDL ERROR] Failed to create renderer: %s",
            SDL_GetError()
        );

        return true;
    }

    bool App::InitTTF()
    {
        SDL_CHECK_C(
            TTF_Init(),
            "[TTF ERROR] Failed to init SDL TTF."
        );

        SDL_CHECK(
            ( m_uiFont = TTF_OpenFont( "Assets/FFFFORWA.TTF", 26 ) ),
            "[TTF ERROR] Failed to load TTF font."
        );
        
        SDL_CHECK(
            ( m_inputFont = TTF_OpenFont( "Assets/FFFFORWA.TTF", 40 ) ),
            "[TTF ERROR] Failed to load TTF font."
        );

        return true;
    }

    bool App::InitWorld()
    {
        //m_world.Init( 4u, m_clientSize );
        m_world.Init( 4u, m_clientSize, LoadTxt( "Assets/instructions.txt", m_bPrintFailureMsg ) );

        SDL_Surface* worldSurface = SDL_CreateSurface(
            Exp_Extent2( m_world.GetGrid().size), SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32
        );

        return true;
    }

    void App::HandleEvents()
    {
        SDL_Event event;

        while ( SDL_PollEvent( &event ) )
        {
            HandleKeyboard( event );

            switch ( event.type )
            {
            case SDL_EVENT_QUIT:
                m_bShouldQuit = true;
                break;

            default:
                break;
            }
        }
    }

    void App::HandleKeyboard(
        const SDL_Event& event
    )
    {
        if ( m_bLoading &&
             SDL_TextInputActive() &&
             event.type == SDL_EVENT_TEXT_INPUT &&
             ! m_bBlockInput )
        {
            m_loadInput += event.text.text;
        }

        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
            m_bShouldQuit = true;
            break;

        case SDLK_SPACE:
            m_bSimulating	   = true;
			m_bPrintFailureMsg = false;
            break;

        case SDLK_r:
            m_bSimulating = false;
            if ( m_loadCache.empty() )
                m_world.Init( 4u, m_clientSize, LoadTxt( "Assets/instructions.txt", m_bPrintFailureMsg ) );
            else
                m_world.Init( 4u, m_clientSize, LoadTxt( m_loadCache, m_bPrintFailureMsg ) );
            break;

        case SDLK_i:
            m_bSimulating = false;
            m_world.Init(4u, m_clientSize, LoadTxt( "Assets/instructions.txt", m_bPrintFailureMsg ));
			m_loadCache.clear();
            break;

        case SDLK_l:
            if ( ! m_bLoading )
            {
                m_bLoading = true;
                m_keyCoolDown = 2.0f;
                SDL_StartTextInput();
                m_bBlockInput = true;
            }
            break;

        case SDLK_RETURN:
            if ( m_bLoading )
            {
                m_world.Init( 4u, m_clientSize, LoadTxt( m_loadInput, m_bPrintFailureMsg ) );
                m_bLoading = false;
				if ( ! m_bPrintFailureMsg )
	                m_loadCache = m_loadInput;
                m_loadInput.clear();
                SDL_StopTextInput();
                m_bBlockInput = true;
            }
            break;

        case SDLK_BACKSPACE:
            if ( m_bLoading &&
                 ! m_loadInput.empty() )
            {
                m_loadInput.pop_back();
                m_bBlockInput = true;
            }

        default:
            break;
        }
    }

    void App::Update()
    {
        m_world.Tick();
    }

    void App::Render()
    {
        SDL_SetRenderDrawColor( m_renderer, Exp_Color_u32( Color{} ) );
        SDL_RenderClear( m_renderer );

        // Render world.
        if ( ! m_bLoading )
        {
            auto& grid     = m_world.GetGrid();
            auto  cellSize = m_world.GetCellSize();

            SDL_FRect def{ 0.0f, 0.0f, cellSize, cellSize };
            List<SDL_FRect> rects( m_worldDrawCountCache, def );
            u32 count = 0u;

            Cell cell;
            while ( grid.Loop( cell ) )
                if ( cell.bAlive )
                {
                    if ( count >= rects.size() )
                        rects.push_back( def );

                    auto& rect = rects[ count++ ];

                    rect.x = cell.coord.x * cellSize;
                    rect.y = cell.coord.y * cellSize;
                }

            SDL_SetRenderDrawColor( m_renderer, 255, 255, 255, 255 );
            SDL_RenderFillRects( m_renderer, rects.data(), (int)rects.size() );
        }
        // Render fonts.
        else
        {
            SDL_Color    txtColor   = { 255, 255, 255 };

            // Render info text.
            String infoText = "Type filename and hit enter to load:";
            SDL_Surface* infoSurface = TTF_RenderText_Solid( m_uiFont, infoText.c_str(), txtColor );
            SDL_Texture* infoTexture = SDL_CreateTextureFromSurface( m_renderer, infoSurface );

            SDL_FRect dst{};
            {
                dst.x = (f32)m_clientSize.width * 0.5f - (f32)infoSurface->w * 0.5f;
                dst.y = (f32)m_clientSize.height * 0.25f;
                dst.w = (f32)infoSurface->w;
                dst.h = (f32)infoSurface->h;
            }

            SDL_RenderTexture( m_renderer, infoTexture, nullptr, &dst );

            // Render input text.
            if ( ! m_loadInput.empty() )
            {
                SDL_Surface* txtSurface = TTF_RenderText_Solid( m_inputFont, m_loadInput.c_str(), txtColor );
                SDL_Texture* txtTexture = SDL_CreateTextureFromSurface( m_renderer, txtSurface );

                {
                    dst.x = (f32)m_clientSize.width  * 0.5f - (f32)txtSurface->w * 0.5f;
                    dst.y = (f32)m_clientSize.height * 0.5f - (f32)txtSurface->h * 0.5f;
                    dst.w = (f32)txtSurface->w;
                    dst.h = (f32)txtSurface->h;
                }

                SDL_RenderTexture( m_renderer, txtTexture, nullptr, &dst );

                SDL_DestroyTexture( txtTexture );
                SDL_DestroySurface( txtSurface );
            }
        }

		if ( m_bPrintFailureMsg )
		{
			SDL_Color txtColor = { 255, 0, 0 };

			String		 errorMsg	  = "Failed to load file from disc.";
			SDL_Surface* errorSurface = TTF_RenderText_Solid( m_uiFont, errorMsg.c_str(), txtColor );
			SDL_Texture* errorTexture = SDL_CreateTextureFromSurface( m_renderer, errorSurface );
			
            SDL_FRect dst{};
            {
                dst.x = (f32)m_clientSize.width * 0.5f - (f32)errorSurface->w * 0.5f;
                dst.y = (f32)m_clientSize.height * 0.25f;
                dst.w = (f32)errorSurface->w;
                dst.h = (f32)errorSurface->h;
            }

            SDL_RenderTexture( m_renderer, errorTexture, nullptr, &dst );

            SDL_DestroyTexture( errorTexture );
            SDL_DestroySurface( errorSurface );
		}

        SDL_RenderPresent( m_renderer );
    }

}