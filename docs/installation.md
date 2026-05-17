---
title: Installation
description: Install aetherwm on AerynOS, Arch, Fedora, Gentoo, Guix System, NixOS, PikaOS, or build from source.
---

## Package Installation

aetherwm is available as a pre-built package on several distributions. Choose your distribution below.

---

### AerynOS

aetherwm is available in the **AerynOS package repository**.

You can install it using the `moss` package manager:

```bash
sudo moss install aetherwm
```

---

### Arch Linux

aetherwm is available in the **Arch User Repository (AUR)**.

You can install it using an AUR helper like `yay` or `paru`:

```bash
yay -S aetherwm-git
```

> **Tip:** This package pulls the latest git version, ensuring you have the newest features and fixes.

---

### Fedora

The package is in the third-party **Terra repository**. First, add the Terra Repository.

> **Warning:** Both commands require root privileges. Use `sudo` if needed.

```bash
dnf install --nogpgcheck --repofrompath 'terra,https://repos.fyralabs.com/terra$releasever' terra-release
```

Then, install the package:

```bash
dnf install aetherwm
```

---

### Gentoo

The package is hosted in the community-maintained **GURU** repository.

1. **Add the GURU repository**

   ```bash
   emerge --ask --verbose eselect-repository
   eselect repository enable guru
   emerge --sync guru
   ```

2. **Unmask packages**
   Add the required packages to your `package.accept_keywords` file:
   - `gui-libs/scenefx`
   - `gui-wm/aetherwm`

3. **Install aether**
   ```bash
   emerge --ask --verbose gui-wm/aetherwm
   ```

---

### Guix System

The package definition is described in the source repository.

1. **Add aether channel**
   Add to `$HOME/.config/guix/channels.scm`:

   ```scheme
   (cons (channel
           (name 'aetherwm)
           (url "https://github.com/aetherwm/aether.git")
           (branch "main"))
         %default-channels)
   ```

2. **Install**
   After running `guix pull`, you can install aetherwm:

   ```bash
   guix install aetherwm
   ```

   Or add it to your system configuration using the aetherwm module:

   ```scheme
   (use-modules (aetherwm))

   (packages (cons*
               aetherwm-git
               ... ;; Other packages
               %base-packages))
   ```

> **Tip:** For more information, see the [Guix System documentation](https://guix.gnu.org/manual/devel/en/html_node/Channels.html).

---

### NixOS

The repository provides a Flake with a NixOS module.

1. **Add flake input**

   ```nix
   # flake.nix
   {
     inputs = {
       nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
       aetherwm = {
         url = "github:aetherwm/aether";
         inputs.nixpkgs.follows = "nixpkgs";
       };
       # other inputs ...
     };
   }
   ```

2. **Import the NixOS module**

   **Option A — Import in `configuration.nix`:**

   ```nix
   # configuration.nix (or any other file that you import)
   {inputs, ...}: {
     imports = [
       inputs.aetherwm.nixosModules.aether
       # .. other imports ...
     ];

     # ...
   }
   ```

   **Option B — Import directly in flake:**

   ```nix
   # flake.nix
   {
     # ...

     outputs = { self, nixpkgs, aetherwm, ...}@inputs: let
       inherit (nixpkgs) lib;
       # ...
     in {
       nixosConfigurations.YourHostName = lib.nixosSystem {
         modules = [
           aetherwm.nixosModules.aether # or inputs.aetherwm.nixosModules.aether
           # other imports ...
         ];
       };
     }
   }
   ```

3. **Enable the module**

   ```nix
   # configuration.nix (or any other file that you import)
   {
     programs.aether.enable = true;
   }
   ```

4. **Start aether on login**

   The following are common examples. Refer to the official NixOS documentation for full configuration options.

   **Option A — greetd:** Autologin on first start; login screen after logout.

   ```nix
   services.greetd = {
     enable = true;
     settings = {
       initial_session = {
         command = "aether";
         user = "your-username"; # auto-login on first start, no password required
       };
       default_session = {
         command = "${pkgs.greetd.tuigreet}/bin/tuigreet --cmd aether";
         user = "greeter";
       };
     };
   };
   ```

   **Option B — Display manager autologin:** Autologin via an existing display manager (e.g. SDDM, GDM). [`addLoginEntry`](/docs/nix-options#addloginentry) (default: `true`) automatically registers aether as a session.

   ```nix
   services.displayManager = {
     defaultSession = "aether"; # derived from aether.desktop filename
     autoLogin = {
       enable = true;
       user = "your-username";
     };
   };
   ```

   **Option C — getty autologin:** No login screen, boots directly into aether on TTY1.

   For bash/zsh:

   ```nix
   services.getty.autologinUser = "your-username";

   environment.loginShellInit = ''
     [ "$(tty)" = /dev/tty1 ] && exec aether
   '';
   ```

   For fish:

   ```nix
   services.getty.autologinUser = "your-username";

   programs.fish.loginShellInit = ''
     if test (tty) = /dev/tty1
         exec aether
     end
   '';
   ```

5. **All available options**

   See [Nix Module Options](/docs/nix-options) for the full list of NixOS and Home Manager options.

---

### PikaOS

aetherwm is available in the **PikaOS package repository**.

You can install it using the `pikman` package manager:

```bash
pikman install aetherwm
```

---

## Building from Source

If your distribution isn't listed above, or you want the latest unreleased changes, you can build aetherwm from source.

> **Info:** Ensure the following dependencies are installed before proceeding:
>
> - `wayland`
> - `wayland-protocols`
> - `libinput`
> - `libdrm`
> - `libxkbcommon`
> - `pixman`
> - `libdisplay-info`
> - `libliftoff`
> - `hwdata`
> - `seatd`
> - `pcre2`
> - `xorg-xwayland`
> - `libxcb`

You will need to build `wlroots` and `scenefx` manually as well.

1. **Build wlroots**
   Clone and install the specific version required (check README for latest version).

   ```bash
   git clone -b 0.19.3 https://gitlab.freedesktop.org/wlroots/wlroots.git
   cd wlroots
   meson build -Dprefix=/usr
   sudo ninja -C build install
   ```

2. **Build scenefx**
   This library handles the visual effects.

   ```bash
   git clone -b 0.4.1 https://github.com/wlrfx/scenefx.git
   cd scenefx
   meson build -Dprefix=/usr
   sudo ninja -C build install
   ```

3. **Build aetherwm**
   Finally, compile the compositor itself.
   ```bash
   git clone https://github.com/aetherwm/aether.git
   cd aether
   meson build -Dprefix=/usr
   sudo ninja -C build install
   ```
