```sh
meson setup build
meson compile -C build
meson install -C build
```
meson setup build --prefix=/usr --buildtype=release
meson compile -C build
```
sudo cp conf/config.conf /etc/vaxp/aether/
sudo setfacl -R -d -m o::rw,g::rw,u::rw /etc/vaxp/aether/
sudo chmod 666 /etc/vaxp/aether/config.conf
```

# التأكد من وجود مجلد subprojects
mkdir -p subprojects

# استنساخ مستودع wlroots بالإصدار المطابق 0.19 داخل مجeld subprojects
git clone https://gitlab.freedesktop.org/wlroots/wlroots.git -b 0.19 subprojects/wlroots
```

```
sudo apt update && sudo apt install -y \
    xwayland \
    liblcms2-dev \
    libxcb-ewmh-dev \
    libxcb-icccm4-dev \
    libxcb-render-util0-dev \
    libxcb-xinput-dev \
    libxcb-composite0-dev
```


مرحبا هذا مشروع aetherمشروع ملحن وايلاند وهو ملحن متطور ومكتمل هنالك بعض المسات البسيطه التي اريد تحديثها فيه اهمهم واولهم هي gappovاريد تحديثها وفصلها الى اثنين هي حاليا تتحكم بل gap من اعلى ومن اسفل اريد فصلها الى ثنين بحيث تصبح لدينا واحده لتوب وواحده للبتوم 