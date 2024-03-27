# ProjectLocker v4.1
シンプルなBattlefield 4の外部チート。  
ProjectLocker v4.1では、v3.*からv4.0で失われたいくつかの機能を復活させ、新たにDamageHackを追加しました。  
BulletPerShotの値を編集しているので、[ 銃弾1発当たりのダメージ * Value = 敵に与えるダメージ ] となります。  
ショットガン（散弾）では十数発を同時に発射しているので、1桁に設定すると逆にダメージが落ちるので要注意です。

## 機能
### AimBot
* エイム予測、VehicleAimBot等なし
* 最適化不足
* mouse_eventを使用
#### ESP
* Soldier ESP : 2DBox, BoxFilled, Line, HealthBar, Distance, Name, VisCheck
* Vehicle ESP : 3D Box
#### Misc
* StreamProof
* Spectator NameList
* NoRecoil/Spread
* DamageHack
* UnlockAll
* Config Load/Save

## Q&A
Q. PBSS Bypassはしてあるのか？  
A. 外部チートなので基本的には不要です。実際未検出の時には100時間近くBANされてないしね。

Q. Detect?  
A. BF4のアンチチートはプログラムのSigベースの検出を行っているので、コードの書き換えやVMProtectやEnigma Protector等を使用してSigベースの検出ができないようにしてください。  

## メディア
![image](https://github.com/TheKawaiiNeko/BF4-External/assets/159750768/1ae0bf8b-decd-493a-afc6-704e0cf6a2ef)
![image](https://github.com/TheKawaiiNeko/BF4-External/assets/159750768/9f3e069e-572f-4695-8e1b-8409fae52f05)
