#include <App.h>

int main()
{
    Goi::App app;
    if ( app.Init( "Game of Life", Goi::EAspectRatio::k16_9, 1080 ) )
        app.Run();
    app.Quit();

    return 0;
}