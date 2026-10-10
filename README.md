# KWin with command-only swipe gestures

This is an unofficial fork of [KDE KWin](https://github.com/KDE/kwin), based on **v6.7.5**, with a focused patch for immediate, one-shot swipe commands. The repository is named `nwin-wm`, but the compositor, package, SDK, configuration files and D-Bus interfaces retain their standard **KWin** identities.

## Gesture behavior

- A swipe recognizes its initial dominant direction after 16 logical units of movement. Small diagonal noise is ignored; equal horizontal and vertical movement selects the vertical direction.
- A matching command executes immediately, before finger lift. Further movement, reversal or cancellation cannot repeat or undo it.
- Eligible bindings are captured before dispatch. Opening Overview cannot also open the desktop grid in the same stroke; use another swipe.
- Touchscreen swipes use the logical movement of the contacts' centroid.
- Swipe progress callbacks are not invoked. Pinch gestures, touch-border feedback, keyboard shortcuts and ordinary transition animations keep their upstream behavior.

## Install the binary package

The [latest release](https://github.com/chiezkeik/nwin-wm/releases/latest) contains **one patched `kwin` package**, not a renamed Plasma distribution.

The published binary is for **Arch Linux x86_64, Plasma 6.7.5, Qt 6.11.2 and libstdc++ 16.2.1 or newer**. Its dependencies enforce the Plasma and Qt versions used for this build. It is not a package for other distributions or arbitrary Plasma versions; rebuild against the matching upstream KWin release when upgrading.

Download `kwin-6.7.5-1.3-x86_64.pkg.tar.zst` and `SHA256SUMS` from the release, then:

```sh
sha256sum -c SHA256SUMS
sudo pacman -U ./kwin-6.7.5-1.3-x86_64.pkg.tar.zst
```

Log out and select the normal **Plasma (Wayland)** session. Keep the stock Plasma clients and addons; no other patched packages or new session entries are needed. An official KWin upgrade can replace this patch. To return to the distribution packages, use `sudo pacman -Syu kwin`.

The earlier seven-package native NWin release is obsolete and is retained only as historical material. Do not mix those packages with this release; this package targets a stock Plasma installation.

## Build the package

[`packaging/PKGBUILD`](packaging/PKGBUILD) downloads the upstream v6.7.5 tag and applies only [`discrete-swipes.patch`](packaging/discrete-swipes.patch). The patch includes the gesture and compositor integration regressions.

```sh
git clone https://github.com/chiezkeik/nwin-wm.git
cd nwin-wm/packaging
CMAKE_BUILD_PARALLEL_LEVEL=4 makepkg -s
```

`check()` runs the gesture and virtual-desktop unit suites plus the touch-input and discrete-swipe compositor integration suites.

## Release changes: 6.7.5-1.3

- Replaced the renamed compositor/client bundle with directly cloned upstream KWin and one gesture patch.
- Preserved the stock exported KWin SDK symbols and the affected exported class layouts.
- Verified the patched compositor with unmodified Plasma, Cube and Spectacle in an isolated virtual-output Wayland session.

Report fork-specific bugs to [this repository](https://github.com/chiezkeik/nwin-wm/issues), not the upstream KDE bug tracker.

## Upstream KWin

KWin is an easy to use, but flexible, compositor for Wayland on Linux. Its primary usage is in conjunction with a Desktop Shell (e.g. KDE Plasma Desktop). KWin is designed to go out of the way; users should not notice that they use a window manager at all. Nevertheless KWin provides a steep learning curve for advanced features, which are available, if they do not conflict with the primary mission. KWin does not have a dedicated targeted user group, but follows the targeted user group of the Desktop Shell using KWin as it's window manager.

## KWin is not...

 * a standalone Wayland compositor (c.f. labwc, sway) and does not provide any functionality belonging to a Desktop Shell.
 * a replacement for window managers designed for use with a specific Desktop Shell (e.g. GNOME Shell)
 * a minimalistic window manager
 * designed for use with network transparency, though it is possible (with e.g. waypipe).

# Contributing to KWin

Please refer to the [contributing document](CONTRIBUTING.md) for everything you need to know to get started contributing to KWin.

# Contacting KWin development team

 * IRC: #kde-kwin on irc.libera.chat
 * Matrix: [#kwin:kde.org](https://go.kde.org/matrix/#/#kwin:kde.org)

# Support
## Application Developer
If you are an application developer having questions regarding windowing systems (either X11 or Wayland) please do not hesitate to contact us.

## End user
Please contact the support channels of your Linux distribution for user support. The KWin development team does not provide end user support.

# Reporting bugs

Please use [KDE's bugtracker](https://bugs.kde.org) and report for [product KWin](https://bugs.kde.org/enter_bug.cgi?product=kwin).

## Guidelines for new features

A new Feature can only be added to KWin if:

 * it does not violate the primary missions as stated at the start of this document
 * it does not introduce instabilities
 * it is maintained, that is bugs are fixed in a timely manner (second next minor release) if it is not a corner case.
 * it works together with all existing features
 * it supports both single and multi screen
 * it adds a significant advantage
 * it is feature complete, that is supports at least all useful features from competitive implementations
 * it is not a special case for a small user group
 * it does not increase code complexity significantly
 * it does not affect KWin's license (GPLv2+)

All new added features are under probation, that is if any of the non-functional requirements as listed above do not hold true in the next two feature releases, the added feature will be removed again.

The same non functional requirements hold true for any kind of plugins (effects, scripts, etc.). It is suggested to use scripted plugins and distribute them separately.
