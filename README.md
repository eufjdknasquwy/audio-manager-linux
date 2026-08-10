# 🎵 Audio Manager

A simple audio control program for Linux (C# + GTK)

## Who is this for?

For people who switched to a tiling window manager (bspwm, i3, awesome, etc.) and don't want to bother with writing their own audio control program.

Just install it — and it works.

---

## Features

- 🔊 Select output device (speakers, headphones)
- 🎤 Select input device (microphone)
- 📊 Volume control

## Dependencies

- `bash`
- `pulseaudio`

---

## Installation

```bash
# Build from source
1. git clone https://github.com/eufjdknasquwy/audio-manager-linux.git
2. cd audio-manager-linux
3. dotnet publish -c Release -r linux-x64 --self-contained true -p:PublishSingleFile=true
4. ./bin/Release/net8.0/linux-x64/publish/audio-manager
```

## Usage
./audio-manager          # Normal launch
./audio-manager -t       # Start in tray
./audio-manager -i       # Open on microphone tab
./audio-manager -o       # Open on output tab




# 🎵 Audio Manager for linux

Простая программа для управления звуком в Linux (C# + GTK)

## Для кого этот проект?

Для людей, которые перешли на тайлинговый менеджер окон (bspwm, i3, awesome и т.д.) и не хотят заморачиваться с написанием своей программы для управления звуком.

Просто установил — и работает.

---

## Возможности

- 🔊 Выбор динамиков, наушников
- 🎤 Выбор миркофона
- 📊 Регулировка громкости

---

## Установка

```bash
# Собрать из исходников
1. git clone https://github.com/eufjdknasquwy/audio-manager-linux.git
2. cd audio-manager-linux
3. dotnet publish -c Release -r linux-x64 --self-contained true -p:PublishSingleFile=true
4. ./bin/Release/net8.0/linux-x64/publish/audio-manager
```

## Использование
./audio-manager          # Обычный запуск
./audio-manager -t       # Сразу в трей
./audio-manager -i       # Открыть на вкладке микрофонов
./audio-manager -o       # Открыть на вкладке динамиков
