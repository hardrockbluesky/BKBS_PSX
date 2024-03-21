# Project Notes:
Even official rhythm games on PS1 are janky, and developing one now, having to consider the CD implementation of every emulator and device capable of playing it, took a lot of my time and I don't recommend it unless you want to extend what I have here. Even a "good" implementation using system timers instead of tick like I did is going to have the same problems.

There is a lot of rough code in here, some of that is me learning C, some of that is because I chose not to trust the compiler. A lot of the time I would've preferred function calls or simple math but I didn't trust the 90's PSY-Q to not waste precious instructions so I hardcoded and copied boilerplate. The PS1 CPU is a dog and "Goalline.y - noteTargetOffset" potentially getting evaluated every frame was more of a hazard IMO than slightly harder to read code that just uses the value 83. If you're working with a modern compiler I wouldn't worry about it.

# General PSX Development Advice:

Check my list of resources at the bottom and hang out at the PSXDev Discord. Some additional things I learned that you might want to know:

* Avoid dividing at runtime.
* Multiplying by a float value is also bad but I believe that it's not *as* bad.
* Avoid floats in general, there's no hardware support.
* Instead of floats, use fixed point math. The very basic version of it is this: 1 = 4096, so .5 = 2048. Using bitshifts you can quickly convert values to a higher value (for higher data resolution), then shift back to the value range you need to use. The PSY-Q libs use 12 bit fixed point a lot, so get used to it and you'll find it's a neat and easy trick to get more mileage out of the turtle CPU.
* Learn to use CLUTs with either the PSY-Q TIMTOOL or LameGuy64's img2tim, real hardware GPU seems to be way slower than any emulator including mister, and a big part of that is texture cache - the lower bit encoding you use on your textures, the more of them fit in the cache, making drawing them way faster. I had to convert my 16-bit non-CLUT TIMs down to 4-bit to get a stable frame rate.

# LibGS and TMD Notes:
LibGS and TMD models are high level-ish libraries provided with PSY-Q. They have some annoying limitations, but I wanted to add to the evidence that you can make something fun and functional without needing your own model format and poly handler code. There are a lot of promising projects that seem to stall because people waste their passion trying to do things "the right way". We've got limited time and energy. If writing a custom renderer is what you really want to do, do it! I wanted to and I did elsewhere! But if what you want is just to *make a game*, you'll have plenty of work to do on this platform without reinventing the wheel. That said, here are some thoughts on the limitations of GS/TMD and how they should relate to your scope:
* Animation is not easy. You can package multiple models into a TMD and treat them like keyframes or do stepped animation. TMD/GS also has support for parenting and skeletal animation. If you want *skinned* animation, you're out of luck. There are extended/advanced versions of TMD like HMD that might support better animation but I chopped that out of my scope early on, and I suggest you do too until your second or third try.
* TMD/GS do not support distance-based subdivision. The PSX has issues with big polygons for several reasons - texture warping, clipping, z-sort errors. The best looking games often used subdivision as polygons moved closer to the camera to get around these. The blend file I started with had far simpler geometry, and every attempt to clean up visual errors made other visual errors happen. You can see the final difference by looking at the "PSX" versions of the house objects. It took me at least a week from start to finish and there are still problems! LibGS can do auto subdivision of an *entire* TMD model, so you could do chunks like that, but you'll need to understand how to minimize instruction cost by batching pieces of the model together. Your scope lesson from this should be to keep models away from the camera and minimize complex geo overlap, mostly with concave objects. First person or tight third person cameras will murder you and your project.
* I am actually not sure if this is a TMD/GS issue or just the PS1 - when scaled very large, textures get offset by a small amount, up to a full pixel. Using big pixel art for my models was a mistake that I simply didn't have time to clean up the "right" way. If I had more time, I would have pre-scaled all my textures up so one original "pixel" was 2x2 VRAM pixels at least, minimizing the effect of this offset. As is, the standard amount of offset at the edges was enough to ruin my texture placement. You can see how bad it is by turning on PGXP in Duckstation. I had to nudge textures around until it looked *close* to my intention, turn on PGXP to see how far off that was. Basically, another reason to keep models away from the camera, and a warning about something that is nearly undocumented, probably due to the lack of pixel poly games in this style. They were trying to reduce visible pixels back then!

To summarize:
* Avoid animation, use parent-child animation if necessary
* Keep the camera away
* Use simple, non-overlapping geo
* Do not assume pixel perfect texture alignment
* Not previously mentioned, but TMD's are kinda big and you don't have much RAM. You won't be making Ocarina of Time levels.
* Also not previously mentioned, but in LibGS, fog and lighting are tied together. If you want to drop lighting for performance reasons and go unlit, you cannot use fog. You can't do anything fun with vert color at runtime either!

Given enough time, you could make Einhander with TMD/LibGS. You could make a clone or upgrade of basically any arcade game released before Ridge Racer, and many after. If you figure out parented anims, you could even make Tekken or a lot of FF7. What you should avoid are the most popular game styles today - third person "over the shoulder" cameras, first person games in realistic tight spaces. For those you will *need* to drop libGS and use libGPU and libGTE if you don't want them to look terrible. As a side note, if you *do* want to experiment with your own render/sort, know that "READTMD()" is slow as hell and not intended for runtime, you should drop TMD's entirely in favor of your own model format.

# Helpful resources
In no particular order:
* [Sparkletron's PSY-Q Examples](https://github.com/sparkletron/PSYQ_Examples)
* [MBDesigns' PS1 Homebrew Source Code](https://mbdesigns.itch.io/ps1-homebrew-source-code-rendering-3d-graphics-with-psyq) (his blender plugin for RSD output is in here, too)
* [ABelliqueux's nolibgs Examples](https://github.com/ABelliqueux/nolibgs_hello_worlds)
* [LameGuy64's Tutorials](http://lameguy64.net/tutorials/pstutorials/)
* [Arthus's incredible collection of docs](https://psx.arthus.net/sdk/), most importantly everything in [this folder](https://psx.arthus.net/sdk/Psy-Q/DOCS/), especially LibOver47.pdf.
