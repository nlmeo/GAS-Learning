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

- 属性 Widget 及子控件的属性匹配，同时监听当前值和最大值变化并通知蓝图更新界面。
- 技能等级设置、递增接口，以及生命值变化触发的角色死亡和重生状态框架。

- 敌人服务器端寻敌、移动追击、转向和攻击循环；死亡时停止搜索，重生后恢复。
- 近战动画通知球形检测、通用范围检测与伤害事件工具函数。
- 可复制投射物、服务器伤害结算和命中特效多播。
- 玩家受击、死亡、次要技能及 Gameplay Cue 配套资源；属性 UI 接收旧值以支持变化反馈。
- 敌人技能、动画与 GameplayEffect 按近战、远程和角色类别整理。

## 阶段归档

- [2026-11-24：敌人战斗与资源整理](Docs/Archive/2026-11-24-combat.md)

## 开发环境

- Unreal Engine 5.7
- 支持 UE 5.7 的 Visual Studio C++ 工具链
- GameplayAbilities 插件（已在项目文件中启用）

使用 `GAS_Learn.uproject` 生成 IDE 项目文件，构建 `GAS_LearnEditor` 的 Win64 Development 配置后打开项目。

`Binaries`、`Intermediate`、`Saved` 和本地 IDE 缓存由工具生成，不纳入版本控制。

## 资源范围

`Content/` 包含项目蓝图、地图及 Paragon 等二进制资源，`.uasset` 和 `.umap` 使用 Git LFS 管理。克隆前请安装 Git LFS；克隆后运行 `git lfs pull` 下载资源。
