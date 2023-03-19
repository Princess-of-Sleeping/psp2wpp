# psp2wpp

A PS Vita home background wave add plugin.

You can install this plugin and add one custom wave (as name implies: wave++).

~~**Currently only "HENkaku Ensõ" is supported without permanent installation** (because SceShell checks idstorage on boottime only).~~

~~But if you already have the pre-theme color flag in idstorage you can use this plugin even if you don't have HENkaku Ensõ.~~

Now supports non-ensõ by overwritten SCE original waveparams.

<br>
Example image of pinky color wave and flat blue sky.

![image0.png](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/preset/image0.png)

# Installation for Ensõ User

1. Add the plugin path under `*main`.

2. Copy [waveparam.bin](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/preset/waveparam.bin) to `ux0:data/waveparam.bin` or `sd0:data/waveparam.bin` (or `host0:data/waveparam.bin` if `host0:` is available).

3. **(OPTIONAL)** If you use text-based waveparam, copy [waveparam.txt](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/preset/waveparam.txt) to `ux0:data/waveparam.txt` or `sd0:data/waveparam.txt` (or `host0:data/waveparam.txt` if `host0:` is available).

Note

- `waveparam.txt` takes precedence over `waveparam.bin`. If `waveparam.txt` does not exist, parameters from `waveparam.bin` are used.

- Wave can cause glitches if there are many bad values in the parameters, but it's not dangerous.

# Installation for Non Ensõ User

1. Add the plugin path under `*main`.

2. Copy your waveparams to `ux0:/data/waveparams/`. The file naming convention is `${number}.txt`. `${number}` is a number from 0 to 30.

Note

- These are instructions for Non-Ensõ, but are still available for Ensõ.

# Advanced permanent installation

You can permanently install a custom wave by copying `waveparam.bin` to `pd0:wave/` and setting idstorage to the appropriate value. The custom wave will persist even without HENkaku.

# Wave find support

## Sample for waveparam

- [list.txt](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/color/list.txt) contains the parameters in the same order as the home edit theme color select (should be). Parameters with listed named are available in [list](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/color/list/).

Many values of waveparam are between 0.0000 and 1.0000. There are rare large values such as 10.0000.

See [cvt.txt](https://github.com/Princess-of-Sleeping/psp2wpp/blob/master/color/cvt.txt) to convert integer type RGB (0x123456) to float type RGB (0.070588, 0.203921, 0.337254).

You can reload `waveparam.txt` while editing txt with VitaShell using [this application](https://github.com/Princess-of-Sleeping/psp2wpp-reload/) ([direct vpk link](https://github.com/Princess-of-Sleeping/psp2wpp-reload/releases/download/v1.0/psp2wpp_reload.vpk)).

# Known issue

Only one custom wave can be added.

  We "hope" to add multiple waves in the future, but this is difficult to reach due to technical issues.

  brief details: The wave param list is statically placed in the SceShell data segment and currently only one is available. This plugin makes use of that space. ~~A simple workaround is to overwrite the SCE original waveparams.~~

  You can now apply up to 32 custom waveparams by overwritten the SCE original waveparams.

# TODO

Add `waveparam.bin` making code.
