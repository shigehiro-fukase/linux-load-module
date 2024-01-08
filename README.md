# linux-load-module

Linuxでカーネルモジュールファイルをロードするサンプルコード。

## usage
```
ldko.elf [OPTIONS] FILE

Load module file (*.ko).

FILE               module image file (*.ko)

OPTIONS
-h, --help         Show this message
--system           Set load modlue method: insmod command via system()
--init_module      Set load modlue method: init_module() (default)
--finit_module     Set load modlue method: finit_module()
```

## 使用例

使用するメソッドとモジュールファイルを指定します。

```
# ldko.elf --init_module MODULE.ko
```

注）`--finit_module` は多くの環境で失敗するようです。


