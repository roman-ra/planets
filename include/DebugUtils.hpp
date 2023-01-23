#pragma once

namespace planets
{
    struct DrawStats
    {
        int lights{0};
        int staticMeshes{0};
        int drawCalls{0};

        void reset(){
            lights = 0;
            staticMeshes = 0;
            drawCalls = 0;
        }
    };   
}
