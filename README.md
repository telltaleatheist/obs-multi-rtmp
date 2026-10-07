# obs-multi-rtmp — fork with stream profiles

This is a fork of [sorayuki/obs-multi-rtmp](https://github.com/sorayuki/obs-multi-rtmp). It adds
**stream profiles**, like vMix streaming presets:

- A **Stream profile** picker at the top of the Multiple output dock. A profile is a named set of
  destinations (for example "Wednesday" = Twitch + one YouTube channel).
- Picking a profile shows only its destinations, and **Start all / Stop all** start and stop exactly
  those. Each destination keeps its own Start button.
- Destinations are stored once and can belong to any number of profiles; deleting a profile never
  deletes a destination.
- Profiles are saved in the plugin's existing per-OBS-profile settings file, in a section the
  original plugin ignores, so the file stays compatible both ways.

Builds against OBS 32.2 with the upstream build files (`cmake --preset windows-x64`). Everything
below is the upstream README.

---

# [Homepage / 主页](https://sorayuki.github.io/obs-multi-rtmp)

## 为什么首页是日语？ / Why is the homepage in Japanese?

因为最初是做给管人用的。

Because it's originally made for virtual Youtubers (VTubers).

# 声明 

近日发现百度贴吧有个叫 maggot 的用户在售卖此插件。咸鱼上也有，没得救了。 

本插件免费使用，作者不收取费用。 

举报之后百度贴吧找我要软件著作权证明，累不爱。 


# Announcement

This plugin is provided for free, without a fee. 

Recently a Baidu Tieba account 'maggot' is selling this plugin. Please, don't buy it.


# お知らせ

本プラグインは無償で提供されるものです。

最近、Baidu Tiebaに「maggot」というアカウント名のユーザーがこのプラグインを販売する行為をしています。

決して購入はしないでください。


# Donate

如果你觉得这个工具很有用想要捐赠，这里是链接。注意：这不是提需求的渠道。

このツールの開発に支援もとい投げ銭をしたいと思った方は以下のリンクからお願いします。(機能のリクエストは受け付けていません)

If you find this tool useful and want to doante, here is the link. (Please do not donate for feature requests.)

## [paypal / 贝宝](https://paypal.me/sorayuki0)

## alipay / 支付宝

![alipay](./docs/zhi.png) 

## wechat / 微信
![wechat](./docs/wechat.jpg)

## Build

This project uses obs-plugintemplate.   
Please refer to obs-plugintemplate to understand how it works.
