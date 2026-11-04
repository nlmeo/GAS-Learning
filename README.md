# GAS-Learning
GAS练习项目

基于 Unreal Engine 5.7 的 C++ Gameplay Ability System 学习项目。

## 当前实现

- 玩家通过 PlayerState 持有 ASC，敌人在角色上持有 ASC 和 AttributeSet。
- Health、MaxHealth、Mana、MaxMana 属性及网络复制。
- 通过 GameplayEffect 初始化属性，并授予角色初始技能。
- ASC 和属性初始化委托，以及 Widget 组件的初始化绑定流程。
- 玩家主技能、敌人受击技能、Gameplay Tags 和受击方向计算。
- 为属性初始化和服务器、客户端的 ASC 初始化流程添加学习注释。

Widget 的属性变化监听和界面更新仍是 TODO。

## 开发环境

- Unreal Engine 5.7
- 支持 UE 5.7 的 Visual Studio C++ 工具链
- GameplayAbilities 插件（已在项目文件中启用）

使用 `GAS_Learn.uproject` 生成 IDE 项目文件，构建 `GAS_LearnEditor` 的 Win64 Development 配置后打开项目。

`Binaries`、`Intermediate`、`Saved` 和本地 IDE 缓存由工具生成，不纳入版本控制。

## 资源范围

`Content/` 包含项目蓝图、地图及 Paragon 等二进制资源，`.uasset` 和 `.umap` 使用 Git LFS 管理。克隆前请安装 Git LFS；克隆后运行 `git lfs pull` 下载资源。
