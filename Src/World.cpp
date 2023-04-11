#include <World.h>
#include <SimplexNoise/SimplexNoise.h>
#include <random>

namespace Goi
{

    World::World()
        : m_grid    {}
        , m_cellSize{ 1.0f }
    {
    }

    void World::Init(
        const u32& cellSize,
        const Extent2i& clientSize
    )
    {
        m_grid.size =
        {
            (u32)clientSize.width / cellSize,
            (u32)clientSize.height / cellSize
        };
        m_cellSize = (f32)cellSize;
        m_grid.Fill();
        AliveFromNoise();
    }

    void World::Init(
        const u32& cellSize,
        const Extent2i& clientSize,
        const List<String>& buffer
    )
    {
        m_grid.size =
        {
            (u32)clientSize.width / cellSize,
            (u32)clientSize.height / cellSize
        };
        m_cellSize = (f32)cellSize;
        m_grid.Fill();
        AliveFromBuffer( buffer );
    }

    const Grid& World::GetGrid()
    {
        return m_grid;
    }

    const f32& World::GetCellSize() const
    {
        return m_cellSize;
    }

    void World::Tick()
    {
        Grid newGrid{};
        newGrid.size = m_grid.size;
        newGrid.Fill();

        for ( u32 y = 0u; y < m_grid.size.height; y++ )
            for ( u32 x = 0u; x < m_grid.size.width; x++ )
            {
                auto& oldCell = m_grid.cells[ y ][ x ];
                auto& newCell = newGrid.cells[ y ][ x ];

                newCell.bAlive = oldCell.bAlive;
                newCell.coord = oldCell.coord;

                u32 aliveNeighs = 0u;

                u32 xStart = (u32)std::max( 0, (s32)x - 1 );
                u32 xEnd   = std::min( x + 2u, m_grid.size.width );
                u32 yStart = (u32)std::max( 0, (s32)y - 1 );
                u32 yEnd   = std::min( y + 2u, m_grid.size.height );

                u32 checked = 0u;
                for ( u32 ny = yStart; ny < yEnd; ny++ )
                    for ( u32 nx = xStart; nx < xEnd; nx++ )
                        if ( ! (ny == y && nx == x) )
                        {
                            checked++;

                            if ( m_grid.Get( nx, ny ).bAlive )
                                aliveNeighs++;
                        }

                if ( aliveNeighs < 2u )
                {
                    newCell.bAlive = false;
                }
                else if ( aliveNeighs == 3u )
                {
                    newCell.bAlive = true;
                }
                else if ( aliveNeighs > 3u )
                {
                    newCell.bAlive = false;
                }
            }

        m_grid.cells.clear();
        m_grid = newGrid;
    }

    bool IsAlive( u32 x, u32 y )
    {
        if ( std::max( 0u, 50u - x ) < 3u &&
             std::max( 0u, 50u - y ) < 3u )
            return true;

        if ( std::max( 0u, 10u - x ) < 7u &&
             std::max( 0u, 10u - y ) < 7u )
            return true;

        if ( std::max( 0u, 60u - x ) < 3u && 
             std::max( 0u, 60u - y ) < 3u )
            return true;

        return false;
    }

    void World::AliveFromNoise()
    {
        float value{};

        s32 yOff = m_grid.size.height / 4;
        s32 yMax = yOff + m_grid.size.height / 2;
        s32 xOff = m_grid.size.width / 4;
        s32 xMax = xOff + m_grid.size.width / 2;

        for ( s32 y = yOff; y < yMax; y++ )
            for ( s32 x = xOff; x < xMax; x++ )
            {
                auto& cell = m_grid.Get( x, y );

                value = SimplexNoise::noise( (f32)x, (f32)y );
                //SimplexNoise sn;
                //value = sn.fractal( 4u, (f32)x, (f32)y );
                cell.bAlive = value > 0.5f;
            }
    }

    void World::AliveFromBuffer(
        const List<String>& buffer
    )
    {
        // Find widest line.
        u32 maxWidth{};
        for ( const auto& line : buffer )
            if ( line.length() > maxWidth )
                maxWidth = line.length();

        // Find offsets from grid center.
        u32 yOff = std::max( 0, (s32)( m_grid.size.height / 2 ) - (s32)( buffer.size() / 2 ) );
        u32 xOff = std::max( 0, (s32)( m_grid.size.width / 2 ) - (s32)( maxWidth / 2 ) );

        // Set alive cells.
        u32 y = yOff;
        for ( const auto& row : buffer )
        {
            u32 x = xOff;
            for ( const auto& val : row )
            {
                if (x == 599)
                    int r = 99 / 1001;

                auto& cell = m_grid.Get( x, y );

                if ( val != ' ' &&
                     val != '.' )
                    cell.bAlive = true;

                x++;
                if ( x >= m_grid.size.width )
                    break;
            }
            y++;
            if ( y >= m_grid.size.height )
                break;
        }
    }

    void Grid::Fill()
    {
        Cell def = { false, {} };
        auto row = List<Cell>( size.width, def );
        cells    = List<List<Cell>>( size.height, row );

        PrepareGrid();
    }

    void Grid::PrepareGrid()
    {
        for ( s32 y = 0u; y < size.height; y++ )
            for ( s32 x = 0u; x < size.width; x++ )
            {
                auto& cell = Get( x, y );

                cell.coord = { x, y };
            }
    }

    bool Grid::Loop(
        Cell& cell
    ) const
    {
        if ( bLooping &&
             loopCoords.y == size.height )
        {
            bLooping   = false;
            loopCoords = { 0, 0 };
            return false;
        }
        else if ( !bLooping )
            bLooping = true;

        auto retCoords = loopCoords;
        loopCoords.x++;
        if ( loopCoords.x == size.width )
        {
            loopCoords.y++;
            loopCoords.x = 0;
        }

        cell = cells[ retCoords.y ][ retCoords.x ];
        return true;
    }

}