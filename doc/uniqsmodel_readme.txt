uniqsmodel��һ��model�㣨���ݲ㣩�Ĺ��ߣ�ʹ��һ��xml�����ļ����������model���󣬿���Ƕ��Ҳ����ʹ��stl����Ƕ�ס�

1.�����ļ�˵����������uniqsmodel/bin/testmodel.xml��
1.1�ο������ļ�
[[[[[
<uniqsproto name="uniqsmodelfucktolua" protobuf_namespace="" description="uniqsmodel��������">
	<!-- includes����Ҫ���Բ�Ҫ��Ŀǰֻ֧��һ��Ƕ�� -->
	<includes>
		<include name="testinclude.h" description="ͨ�ö���"/>
	</includes>
	<!-- defines����CPP���Ե�#define������enumö�٣���һ���ַ�����һ��intֵ��Ӧ�������ظ� -->
	<defines>
		<define name="TEST_DEFINE1" value="3" description="���Զ���1"/>
		<define name="TEST_DEFINE2" value="4" description="���Զ���2"/>
		<define name="TEST_Inner_MaxCount" value="5" description="�ڲ�����������"/>
		<define name="TEST_MaxCount1" value="6" description="�����������1"/>
		<define name="TEST_MaxCount2" value="7" description="�����������2"/>
	</defines>
	<!-- �Զ���ṹ�壬������Э�� -->
	<structs>
		<struct name="Item" description="��Ʒ��">
			<property name="itemid" type="int64" description="��ƷID"/>
			<property name="param1" type="int32" description="����1"/>
		</struct>
		<struct name="Player" description="�����">
			<property name="playerid" type="uint64" description="���ID"/>
			<property name="level" type="int32" description="��ҵȼ�"/>
			<property name="playername" type="string" description="�����"/>
			<property name="item" type="Item" description="ĳ�������"/>
			<property name="items" type="Item" complex_type="map" key="itemid" key_type="int64" max="TEST_Inner_MaxCount" description="��ҵ�������Ʒ"/>
			<property name="equips1" type="int32" complex_type="map" key_type="int32" max="TEST_Inner_MaxCount" description="����װ��1"/>
			<property name="equips2" type="uint64" complex_type="map" key_type="uint64" max="TEST_Inner_MaxCount" description="����װ��2"/>
		</struct>
		<struct name="DataCenter" singleton="true" description="��������">
			<property name="players" type="Player" complex_type="map" key="playerid" key_type="int64" max="TEST_Inner_MaxCount" description="�������"/>
		</struct>
	</structs>
</uniqsproto>
]]]]]
1.2�����ļ�˵��
rawtype:int8/uint8/int16/uint16/int32/uint32/int64/uint64/string

uniqsprotoд���ġ�
uniqsproto.name��������tolua_***_open�ĺ������Լ�tolua_***.h tolua_***.cpp���ļ���
uniqsproto.protobuf_namespace����pb�������ռ�
���е�description:����
uniqsproto.includes.include:���ɵİ���ͷ�ļ�
uniqsproto.defines.define:��uniqsproto���ݹ�����һЩ�򵥵Ķ���
uniqsproto.structs.struct:���ն����һ����Model����
uniqsproto.structs.struct.name:Model���������
uniqsproto.structs.struct.property:Model������������
uniqsproto.structs.struct.property.name:Model�����������Ե�����
uniqsproto.structs.struct.property.type:Model�����������Ե����ͣ�ע�⣬�����map�Ļ�����ֵ�����Ͳ���key�����ͣ�
uniqsproto.structs.struct.property.complex_type:Model�����������Եĸ������ͣ�Ŀǰuniqsproto֧��array/map��uniqsmodelֻ֧��map
uniqsproto.structs.struct.property.key_type:����Model�������������Ǹ������ͣ�Ŀǰuniqsmodelֻ֧��map��ʱ������ʹ��key_type��key_typeֻ����rawtype
uniqsproto.structs.struct.property.max:��ʱû�õ�

2.ʹ�÷���
2.1�����XML��ʽ�Ķ����ļ���ʹ���Ѿ��������uniqsmodel.exe�����ղ����������ɵ��ļ���outputdir/pboutputdir����·�������������Ĭ��ʹ�õ�ǰ·�������ɶ����ļ�
2.2�����toluapp�Ŀ��ļ���toluapp��Դ���иĶ���
WINDOWS����ֱ��ʹ��VS2013+�汾����uniqsmodel/toluapp/toluapp_build/toluapp.sln�ļ����뼴�ɡ�
����ƽ̨���дMAKEFILE������CMAKE�ļ�
2.3��2.1���ɵ��ļ�����������Ŀ�У��������Ŀ��toluapp�����ã�����tolua++.h�Լ�2.2�б������ͷ�ļ���

3.uniqsmodel.exe�������
WINDOWS����ֱ��ʹ��VS2013+�汾����uniqsmodel/uniqsmodel/uniqsmodel.sln�ļ����뼴�ɡ�
����ƽ̨���дMAKEFILE������CMAKE�ļ�

