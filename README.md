# TranslucentTB

[![Liberapay patrons](https://img.shields.io/liberapay/patrons/TranslucentTB.svg)](https://liberapay.com/TranslucentTB/)
[![Join on Discord](https://discordapp.com/api/guilds/304387206552879116/widget.png?style=shield)][Discord]
[![Join on Gitter](https://badges.gitter.im/TranslucentTB/Lobby.svg)][Gitter]
[![Total downloads](https://img.shields.io/github/downloads/TranslucentTB/TranslucentTB/total.svg)](https://github.com/TranslucentTB/TranslucentTB/releases)
[![Build Status](https://dev.azure.com/sylve0n/TranslucentTB/_apis/build/status/TranslucentTB.TranslucentTB?branchName=develop)](https://dev.azure.com/sylve0n/TranslucentTB/_build/latest?definitionId=4&branchName=develop)
[![CodeFactor](https://www.codefactor.io/repository/github/translucenttb/translucenttb/badge/develop)](https://www.codefactor.io/repository/github/translucenttb/translucenttb/overview/develop)
> [**关于恶意软件的指控**, 我们有一个 issues 来解释我们的权限使用。](https://github.com/TranslucentTB/TranslucentTB/issues/350)


轻量级（占用几 MB 内存且几乎不占用 CPU）的实用程序，可使 Windows 任务栏在 Windows 10 上半透明/透明。

您可以在以下图像中查看可以进行自定义的示例：

![blur](https://i.imgur.com/r4ZJjnL.png) ![transparent](https://i.imgur.com/eLGTtwp.png) ![acrylic](https://i.imgur.com/M15IPJW.png)

## 功能

- 高级的 **颜色选择器**，支持 Alpha 和实时预览，可更改任务栏的颜色。
- **任务栏状态** (选择一种 - 可以在除了 “正常” 以外的其他状态自定义颜色):
  - **模糊**: 将使任务栏略微模糊。
  - **透明**: 透明的任务栏。
  - **正常**: 常规Windows风格。（就像 TranslucentTB 没有运行一样）
  - **不透明**: 不透明。
  - **亚克力**: 仅 Windows 10 April 2018 更新及更高版本。将使任务栏的外观类似于 Microsoft 的 Fluent Design 准则。
- **动态** 模式 (可以一起使用，它们每个都提供您可以自定义的任务栏状态和颜色):
  - **动态窗口**: 如果当前窗口最大化，则将任务栏更改为其他外观。
  - **动态开始菜单**: 打开开始菜单时，将更改任务栏外观。
  - **动态小娜**: 打开 Cortana（或禁用 Cortana 的搜索菜单）时，将更改任务栏的外观。
  - **动态时间轴/任务视图**: 打开时间轴（或旧版本的任务视图）时，将更改任务栏外观。
- 能够 **显示或隐藏桌面预览** 按钮。可以自定义为 **总是** 或 **动态**.

您可以在 [此处](https://gfycat.com/TidyFelineCrownofthornsstarfish) (较短) 和 [此处](https://gfycat.com/ConsciousCriminalDassie) (较长) 视频中查看它的运行情况。

## 下载

您可以从 [Microsoft Store](https://www.microsoft.com/store/apps/9PF4KZ2VN4W9) (注：[汉化版](https://www.microsoft.com/zh-cn/p/translucenttb-%E6%B1%89%E5%8C%96-by-tpxxn/9n5w18jc9bg2) 点击此链接) 免费下载该程序，并利用其功能，例如后台自动更新和设置同步。

如果您喜欢经典下载，则可以 [通过 release 选项卡](https://github.com/TranslucentTB/TranslucentTB/releases) 进行下载。

如果您想获得最新的前沿构建，可以在 [Azure Pipelines 页面](https://dev.azure.com/sylve0n/TranslucentTB/_build?definitionId=4) 上进行获取（`Configuration: Release` > `Artifacts` > `TranslucentTB-setup.exe`）。 请注意，这些构建可能无法正常运行，或包含部分完成的功能。 使用风险自负。

## 添加开机启动

要将TranslucentTB添加到启动中，请检查 TranslucentTB 托盘图标的上下文菜单中的 “开机启动” 选项。 如果显示为灰色，则说明已从任务管理器或您的组织禁用 TranslucentTB 启动。

## 捐赠

[我们拥有 Liberapay!](https://liberapay.com/TranslucentTB/) 如果您感谢 TranslucentTB 并愿意支持我们的工作，请毫不犹豫地捐款。

## Security

一些防病毒软件过于急(S)躁(B)，因此它们可能将该程序标记为恶意软件。 然而它并不是！ 超过 20 万名用户安全地下载了此程序。源代码是开放的，您可以自己编译，并且我欢迎任何安全检查。

## 感谢

TranslucentTB 是团队合作！ 这是许多人共同努力的结果：

- [@ethanhs](https://github.com/ethanhs),
- [@sylveon](https://github.com/sylveon),
- [@MrAksel](https://github.com/MrAksel),
- [@denosawr](https://github.com/denosawr),
- [@PFCKrutonium](https://github.com/PFCKrutonium),
- 最后但并非最不重要的是所有 [我们的贡献者](https://github.com/TranslucentTB/TranslucentTB/graphs/contributors)!

感谢 [@dAKirby309](https://github.com/dAKirby309) 制作了图标！您可以在 [他的个人资料](https://dakirby309.deviantart.com/) 上找到更多作品。

### 类似程序

如果您正在寻找的东西不仅可以修改任务栏，还可以使用以下多个程序。

- [Taskbar Tools](https://github.com/Elestriel/TaskbarTools) 是一个用 C# 编写的类似程序。 但是，似乎没有必要。
- [TaskbarX](https://github.com/ChrisAnd1998/TaskbarX) 也可以改变任务栏的外观，但它缺乏 TranslucentTB 的动态模式。

您可能已经从诸如 StartIsBack、Start10 和现已淘汰的 Classic Shell 之类的程序中看到了类似的半透明功能。所有这些都是很棒的程序，但是我不需要 开始菜单替换 功能，所以我写了这个程序。

TranslucentTB 还通过这些程序所不具备的功能（如 动态窗口、动态预览和 动态开始菜单）在任务栏上实现了更高的自定义性。对存储和内存的影响也较小。

### 许可

该程序是 GPL v3 许可下的免费（就像 speech 一样）软件。请参见 [LICENSE.md](LICENSE.md) 文件查看更多信息。

[Discord]: https://discord.gg/TranslucentTB
[Gitter]: https://gitter.im/TranslucentTB/Lobby
