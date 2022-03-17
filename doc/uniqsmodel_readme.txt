uniqsmodel是一个model层（数据层）的工具，使用一个xml定义文件来定义各个model对象，可以嵌套也可以使用stl容器嵌套。

1.定义文件说明：（参照uniqsmodel/bin/testmodel.xml）
1.1参考定义文件
[[[[[
<uniqsproto name="uniqsmodelfucktolua" protobuf_namespace="" description="uniqsmodel测试用例">
	<!-- includes可以要可以不要，目前只支持一级嵌套 -->
	<includes>
		<include name="testinclude.h" description="通用定义"/>
	</includes>
	<!-- defines就是CPP语言的#define，类似enum枚举，把一个字符串和一个int值对应，可以重复 -->
	<defines>
		<define name="TEST_DEFINE1" value="3" description="测试定义1"/>
		<define name="TEST_DEFINE2" value="4" description="测试定义2"/>
		<define name="TEST_Inner_MaxCount" value="5" description="内部类的最大数量"/>
		<define name="TEST_MaxCount1" value="6" description="测试最大数量1"/>
		<define name="TEST_MaxCount2" value="7" description="测试最大数量2"/>
	</defines>
	<!-- 自定义结构体，可用于协议 -->
	<structs>
		<struct name="Item" description="物品类">
			<property name="itemid" type="int64" description="物品ID"/>
			<property name="param1" type="int32" description="参数1"/>
		</struct>
		<struct name="Player" description="玩家类">
			<property name="playerid" type="uint64" description="玩家ID"/>
			<property name="level" type="int32" description="玩家等级"/>
			<property name="playername" type="string" description="玩家名"/>
			<property name="item" type="Item" description="某任务道具"/>
			<property name="items" type="Item" complex_type="map" key="itemid" key_type="int64" max="TEST_Inner_MaxCount" description="玩家的所有物品"/>
			<property name="equips1" type="int32" complex_type="map" key_type="int32" max="TEST_Inner_MaxCount" description="身上装备1"/>
			<property name="equips2" type="uint64" complex_type="map" key_type="uint64" max="TEST_Inner_MaxCount" description="身上装备2"/>
		</struct>
		<struct name="DataCenter" singleton="true" description="数据中心">
			<property name="players" type="Player" complex_type="map" key="playerid" key_type="int64" max="TEST_Inner_MaxCount" description="所有玩家"/>
		</struct>
	</structs>
</uniqsproto>
]]]]]
1.2定义文件说明
rawtype:int8/uint8/int16/uint16/int32/uint32/int64/uint64/string

uniqsproto写死的。
uniqsproto.name用于生成tolua_***_open的函数名以及tolua_***.h tolua_***.cpp的文件名
uniqsproto.protobuf_namespace生成pb的命名空间
所有的description:描述
uniqsproto.includes.include:生成的包含头文件
uniqsproto.defines.define:向uniqsproto兼容过来的一些简单的定义
uniqsproto.structs.struct:最终定义的一个个Model对象
uniqsproto.structs.struct.name:Model对象的名字
uniqsproto.structs.struct.property:Model对象的里的属性
uniqsproto.structs.struct.property.name:Model对象的里的属性的名字
uniqsproto.structs.struct.property.type:Model对象的里的属性的类型（注意，如果是map的话，是值的类型不是key的类型）
uniqsproto.structs.struct.property.complex_type:Model对象的里的属性的复杂类型，目前uniqsproto支持array/map，uniqsmodel只支持map
uniqsproto.structs.struct.property.key_type:仅当Model对象的里的属性是复杂类型（目前uniqsmodel只支持map）时，才能使用key_type且key_type只能是rawtype
uniqsproto.structs.struct.property.max:暂时没用到

2.使用方法
2.1定义好XML格式的定义文件，使用已经编译出的uniqsmodel.exe，按照参数设置生成的文件的outputdir/pboutputdir设置路径（如果不设置默认使用当前路径）生成定义文件
2.2编译出toluapp的库文件（toluapp的源码有改动）
WINDOWS环境直接使用VS2013+版本，打开uniqsmodel/toluapp/toluapp_build/toluapp.sln文件编译即可。
其他平台请编写MAKEFILE或者是CMAKE文件
2.3把2.1生成的文件加入您的项目中，并添加项目对toluapp的引用（包含tolua++.h以及2.2中编译出的头文件）

3.uniqsmodel.exe编译帮助
WINDOWS环境直接使用VS2013+版本，打开uniqsmodel/uniqsmodel/uniqsmodel.sln文件编译即可。
其他平台请编写MAKEFILE或者是CMAKE文件

