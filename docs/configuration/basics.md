---
title: Basic Configuration
description: Learn how to configure aetherwm files, environment variables, and autostart scripts.
---

## Configuration File

aetherwm uses a simple configuration file format. By default, it looks for a configuration file in `~/.config/aether/`.

1. **Locate Default Config**

   A fallback configuration is provided at `/etc/aether/config.conf`. You can use this as a reference.

2. **Create User Config**

   Copy the default config to your local config directory to start customizing.

   ```bash
   mkdir -p ~/.config/aether
   cp /etc/aether/config.conf ~/.config/aether/config.conf
   ```

3. **Launch with Custom Config (Optional)**

   If you prefer to keep your config elsewhere, you can launch aether with the `-c` flag.

   ```bash
   aether -c /path/to/your_config.conf
   ```

### Sub-Configuration

To keep your configuration organized, you can split it into multiple files and include them using the `source` keyword.

```ini
# Import keybindings from a separate file
source=~/.config/aether/bind.conf

# Relative paths work too
source=./theme.conf

# Optional: ignore if file doesn't exist (useful for shared configs)
source-optional=~/.config/aether/optional.conf
```

### Validate Configuration

You can check your configuration for errors without starting aetherwm:

```bash
aether -c /path/to/config.conf -p
```

Use with `source-optional` for shared configs across different setups.

## Environment Variables

You can define environment variables directly within your config file. These are set before the window manager fully initializes.

> **Warning:** Environment variables defined here will be **reset** every time you reload the configuration.

```ini
env=QT_IM_MODULES,wayland;fcitx
env=XMODIFIERS,@im=fcitx
```

## Autostart

aetherwm can automatically run commands or scripts upon startup. There are two modes for execution:

| Command | Behavior | Usage Case |
| :--- | :--- | :--- |
| `exec-once` | Runs **only once** when aetherwm starts. | Status bars, Wallpapers, Notification daemons |
| `exec` | Runs **every time** the config is reloaded. | Scripts that need to refresh settings |

### Example Setup

```ini
# Start the status bar once
exec-once=waybar

# Set wallpaper
exec-once=swaybg -i ~/.config/aether/wallpaper/room.png

# Reload a custom script on config change
exec=bash ~/.config/aether/reload-settings.sh
```
