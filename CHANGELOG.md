# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Auto code conversion when changing a Sketch language ([#150](https://github.com/Sketch-sh/sketch-sh/pull/150) - @no-stack-dub-sack)
- Prettify ReasonML code with refmt via Ctrl-Shift-I shortcut ([#150](https://github.com/Sketch-sh/sketch-sh/pull/150) - @no-stack-dub-sack)
- Clickable Markdown links ([#161](https://github.com/Sketch-sh/sketch-sh/pull/161) - @thangngoc89)
- Ctrl-Shift-Enter shortcut: focus on next block, create a new text block if this is the last block ([#151](https://github.com/Sketch-sh/sketch-sh/pull/151) - @nimish-gupta)
- Code evaluation timeout (useful for preventing infinite loop) ([#154](https://github.com/Sketch-sh/sketch-sh/pull/154) - @thangngoc89)

### Fixed

- Can't navigate between forks
  This is due to the component doesn't react to props changes (uncontrolled component)
  ([#148](https://github.com/Sketch-sh/sketch-sh/pull/148) - @thangngoc89)
