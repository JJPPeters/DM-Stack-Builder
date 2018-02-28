# DM Stack Builder
Plugin for Gatan Digital Micrograph to build a stack from updating images. Used to record live videos or to acquire rapid frames to align and sum later. Currently operates through a menu system. Should be compatible with both GMS 2 and GMS 3.

## Compiling
The only libraries required to compile are Boost (default looks for the system environment variable `BOOST_ROOT`) and the Digital Micrograph SDK (default looks for the system environment variable `DMSDK2`)

## Installation
Simply copy the generated `dm-stack-builder.dll` to `D:\ProgramData\Gatan\Plugins`. The default solution configuration will try to copy it there automatically.

## Usage
The plugin creates a `Stack` menu entry. Under this is a `Images` menu with an option to add the front selected image. Any currently added images will also be shown in this sub menu with an option to remove them. If an image is closed, it is automatically removed. Then clicking `Start` will create a stack image for each monitored image and copy across the data every time the **_final pixel_** changes. Stack building can be stopped by pressing `Stop` in the menu and `Pause`/`Unpause` will pause the stack building and resume it using the same stack.

In the stack image, the global tags are copied at the start of acquisition and a time stamp for every frame is recorded. Settings for options are recorded in the global tags and will kept next time Digital Micrograph is opened.

### Options
The stack can be built in 3 different modes (`Fixed`, `Circular` and `Expanding`) with a set `Stack size`.
 - In the `Fixed` mode, a stack with a number of slices defined by the set `Stack size` is created and filled and the stack builder stops.
 - In the `Circular` mode, a stack with a number of slices defined by the set `Stack size` is created and filled. When the stack is full, it is overwritten, starting from the front of the stack. This is repeated until the builder is stopped.
 - In the `Expanding` mode, a stack with a number of slices defined by the set `Stack size` is created and filled. When the stack is full, the stack is expanded by the same `Stack size`. This is repeated until the builder is stopped.

### Caveats and problems

The plugin has a number of limitations and problems, hopefully some will be fixed, but some probably cannot be overcome.

 - When building stacks in the `Expanding` mode, the expansion requires moving the entire image in the computer's memory. The larger the stack and the larger the image, the longer this process takes. Do not expect anything close to real time videos when using this mode (at least at the frames when the stack was expanded).
 - `Expanding` mode will continue as long as you leave it and will happily consume all your computer's memory and crash it.
 - `Circular` mode does not shift the start point of the stack at all. The first frame is not always the earliest frame recorded, it can be anywhere in the stack. There are timestamps in the tags so the start frame cannot be determined.
 - The microscope tags/calibrations are recorded at the beginning of the stack building. If the magnification is altered during the stack acquisition, this change will not be recorded in the stack.

## Current issues

It crashes Digital Micrograph... Sometimes... This is an issue with stopping the stack building, it seems to work otherwise (though testing has been limited). This will be fixed soon.