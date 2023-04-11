#pragma once

#include <Types.h>

namespace Goi
{

    enum ENeighbour
    {
        kTopLeft,    kTop,    kTopRight,
        kLeft,                kRight,
        kBottomLeft, kBottom, kBottomRight
    };

    struct Cell
    {
        bool            bAlive;
        V2i             coord;
    };

    struct Grid
    {
        Extent2u size;
        List<List<Cell>> cells;

        void Fill();

        Cell& Get( const V2i& coords )
        {
            return cells[ coords.y ][ coords.x ];
        }

        Cell& Get( const u32& x,
                   const u32& y )
        {
            return cells[ y ][ x ];
        }

        bool Loop( Cell& cell ) const;

    private:
        void PrepareGrid();

        mutable bool bLooping;
        mutable V2i  loopCoords;
    };

    class World
    {
    public:
        World();

        void Init( const u32& cellSize,
                   const Extent2i& clientSize );

        void Init( const u32& cellSize,
                   const Extent2i& clientSize,
                   const List<String>& buffer );

        const Grid& GetGrid();

        const f32& GetCellSize() const;

        void Tick();

    private:

        void AliveFromNoise();
        
        void AliveFromBuffer( const List<String>& buffer );

        Grid     m_grid;
        f32      m_cellSize;
    };

}