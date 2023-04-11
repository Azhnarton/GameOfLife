#pragma once

#include <Types.h>
#include <World.h>
#include <Timer.h>

struct SDL_Window;
struct SDL_Renderer;
union  SDL_Event;
struct SDL_Surface;
struct _TTF_Font;
typedef _TTF_Font TTF_Font;

namespace Goi
{

    enum EAspectRatio
    {
        k4_3,
        k16_9
    };

    class App
    {
    public:
        App();

        bool Init( const String& name,
                   const Extent2i& clientSize );

        bool Init( const String& name,
                   const EAspectRatio& aspectRatio,
                   const s32& resolution );

        void Quit();

        void Run();

    private:
        bool InitSDL();
        bool InitTTF();
        bool InitWorld();

        void HandleEvents();
        void HandleKeyboard( const SDL_Event& event );

        void Update();
        void Render();

        String   m_name;
        Extent2i m_clientSize;

#pragma region SDL
        SDL_Window*   m_window;
        SDL_Renderer* m_renderer;
#pragma endregion

#pragma region States
        bool m_bShouldQuit;
        bool m_bSimulating;
        bool m_bLoading;
        bool m_bBlockInput;
#pragma endregion

        World        m_world;
        SDL_Surface* m_worldSurface;
        u32          m_worldDrawCountCache;

        Timer m_timer;
        
        f32 m_keyCoolDown;

        String    m_loadInput;
        String    m_loadCache;
        TTF_Font* m_uiFont;
        TTF_Font* m_inputFont;

		bool m_bPrintFailureMsg;
    };

}