# NWin

NWin is a KWin-based compositor with discrete command swipe gestures. It uses
the `nwin` package, `nwin_wayland` compositor, `nwin_wayland_wrapper` launcher,
`plasma-nwin_wayland.service` session service, and `nwinrc` configuration.
Its D-Bus services are `org.kde.NWin` and `org.kde.NWinWrapper`.
`nwin_wayland --version` identifies the build;
`qdbus6 org.kde.NWin /NWin supportInformation` identifies the running compositor.

NWin is an easy to use, but flexible, compositor for Wayland on Linux. Its primary usage is in conjunction with a Desktop Shell (e.g. KDE Plasma Desktop). NWin is designed to go out of the way; users should not notice that they use a window manager at all. Nevertheless NWin provides a steep learning curve for advanced features, which are available, if they do not conflict with the primary mission. NWin does not have a dedicated targeted user group, but follows the targeted user group of the Desktop Shell using NWin as it's window manager.

## NWin is not...

 * a standalone Wayland compositor (c.f. labwc, sway) and does not provide any functionality belonging to a Desktop Shell.
 * a replacement for window managers designed for use with a specific Desktop Shell (e.g. GNOME Shell)
 * a minimalistic window manager
 * designed for use with network transparency, though it is possible (with e.g. waypipe).

# Contributing to NWin

Please refer to the [contributing document](CONTRIBUTING.md) for everything you need to know to get started contributing to NWin.

# Contacting KWin development team

 * IRC: #kde-nwin on irc.libera.chat
 * Matrix: [#nwin:kde.org](https://go.kde.org/matrix/#/#kwin:kde.org)

# Support
## Application Developer
If you are an application developer having questions regarding windowing systems (either X11 or Wayland) please do not hesitate to contact us.

## End user
Please contact the support channels of your Linux distribution for user support. The KWin development team does not provide end user support.

# Reporting bugs

Please use [KDE's bugtracker](https://bugs.kde.org) and report for [product NWin](https://bugs.kde.org/enter_bug.cgi?product=kwin).

## Guidelines for new features

A new Feature can only be added to NWin if:

 * it does not violate the primary missions as stated at the start of this document
 * it does not introduce instabilities
 * it is maintained, that is bugs are fixed in a timely manner (second next minor release) if it is not a corner case.
 * it works together with all existing features
 * it supports both single and multi screen
 * it adds a significant advantage
 * it is feature complete, that is supports at least all useful features from competitive implementations
 * it is not a special case for a small user group
 * it does not increase code complexity significantly
 * it does not affect NWin's license (GPLv2+)

All new added features are under probation, that is if any of the non-functional requirements as listed above do not hold true in the next two feature releases, the added feature will be removed again.

The same non functional requirements hold true for any kind of plugins (effects, scripts, etc.). It is suggested to use scripted plugins and distribute them separately.

## Swipe gesture semantics

Touchpad and touchscreen swipes execute discrete commands, never finger-controlled
previews. The existing desktop grid, desktop wrapping preference, window layout,
and gesture assignments are unchanged. No scrolling layout or carousel is added.

* Accumulated movement below 16 logical units does nothing. At that boundary the
  dominant axis and initial direction select the command; an exact diagonal tie
  selects the vertical axis.
* The selected eligible command executes synchronously during the recognizing
  motion update, before the fingers lift. Desktop switches and Overview/Grid
  transitions use their normal compositor-owned animations.
* The rest of the stroke is ignored, including long motion, direction reversal,
  and crossing the origin. It cannot repeat the command or execute its opposite.
  Lift the fingers before starting another command swipe.
* Cancellation before recognition does nothing. Cancellation or release after
  recognition cannot undo the command. There is no endpoint projection, velocity
  decision, release-time commit, or rollback.
* All matching bindings snapshot eligibility before any command executes.
  One stroke cannot activate Overview and then continue into Desktop Grid;
  that transition requires a separate stroke.

Pinch gestures and interactive screen-edge gestures retain their existing
behavior. For native swipe consumers, `SwipeGesture::triggered()` denotes
immediate recognition; there are no swipe progress or release feedback APIs.
Scripted `SwipeGestureHandler` emits `activated()` immediately under the same
contract, without preview progress or a cancellation action.

The recognizer regressions are in `autotests/test_gestures.cpp`; run
`QT_QPA_PLATFORM=offscreen ./bin/testGestures` from a configured build directory.

The end-to-end regression `nwin-testDiscreteSwipes` runs a real isolated
compositor, injects touchpad events through the input pipeline, and checks desktop
changes before lift and Overview/Grid command atomicity with a rendered Wayland
window. Run `ctest -R '^nwin-testDiscreteSwipes$' --output-on-failure` from the
build directory. Overview requires an OpenGL renderer and an available render node.

## Arch package

The canonical `nwin/` source and all recipes in `packages/` use the compatible
Arch Plasma release `6.7.5`. `nwin-upstream/` preserves the newer development
checkout; it is not the source of the Arch packages.

The native cutover consists of seven packages:

* `nwin`: compositor, launcher, services, configuration, QML and C++ SDK.
* `nwin-libkscreen`: native output and tablet-mode clients; KScreen ABI unchanged.
* `nwin-plasma-workspace`: native Plasma startup and session integration.
* `nwin-plasma-desktop`: native device settings, effects and desktop controls.
* `nwin-desktop-portal`: additional native portal backend and NWin preference.
* `nwin-spectacle`: native screenshot and window-selection client.
* `nwin-kdeplasma-addons`: native Cube effect, switchers, script and colour picker.

Recipes are in `packages/PKGBUILD` and the corresponding `screen/`, `plasma/`,
`desktop/`, `portal/`, `spectacle/` and `addons/` subdirectories. Their patches
include the complete stable-version migration; the compositor recipe runs the
gesture regressions. Build the native SDK before its client packages.

Install the seven resulting packages together with `sudo pacman -U <packages>`.
Select **Plasma (NWin)** at the next normal login. Do not restart a live Wayland
compositor or log the user out as part of installation.

For an existing setup, copy `kwinrc`, `kwinrulesrc` and `kwinoutputconfig.json`
once to their `nwin*` counterparts only when the destination is absent. Migrate
the native compositor shortcuts from `[kwin]` to `[nwin]`; preserve independent
X11 configuration. NWin does not fall back to old KWin service names or files.

Keep signed official `kwin`, `libkscreen`, `plasma-workspace`, `plasma-desktop`,
`spectacle` and `kdeplasma-addons` packages for rollback. Restore them together
and remove the additional native portal backend before the next normal login.
NWin is a local fork, not an official Arch update: rebuild its matching source
and client packages when changing Plasma, Qt or KDE Frameworks versions.
