
This repository contains the shield files for the [chocofi](https://github.com/pashutk/chocofi) to allow users to build firmware. This can be done by adding the module to the west.yml found in your zmk-config's config directory. There is a full guide available for this here: [ZMK Modules Doc](https://zmk.dev/docs/features/modules)

## Usage

Edit your west.yml file found in your zmk-config's config directory to add the NAME_OF_MODULE module. Example:

```
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    - name: petejoganson
      url-base: https://github.com/petejohanson
    - name: grassfedreeve
      url-base: https://github.com/grassfedreeve
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    - name: ec-support-zmk-module
      remote: petejohanson
      revision: main
    - name: zmk-keyboards-katori
      remote: grassfedreeve
      revision: main
  self:
    path: config
```
Once you have the module added to your west.yml you can then build firmware as if it was in your config's shield directory or in ZM
