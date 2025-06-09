# Mud Engine

## How to run

1. Generate a build `cmake . -B build`
2. To build the executabel `cmake --build build`
3. If successfully built, a `build` folder will contain a sln file

## To Do

### Engine
1. Uniform Buffers &#x2611;
   * Add struct based UBs
   * Allow updating a uniform within the struct and making a draw call
   * Ensure UBs are properly cleaned up

2. Quad Rendering
   * Set position
   * Set size in pixels
   * Set rotation
   * Set scale
  
3. Pixel Perfect Rendering
   * Rendering must be pixel perfect and have correct aspect ratio.
     E.g. if native resolution is 640x360 and window resolution is 1080x1920, ensure the pixels scale correctly and the aspect ratio remaings correct.
     
4. Add time step
5. Add user input
6. Research pixel perfect collision and design a small test level
