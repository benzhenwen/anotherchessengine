#pragma once
#include <cstdint>

namespace Chess::ZobristHashes {

    static uint_fast64_t piece_codes[64][6][2] = {
        {
            { 0x2eecfe4d88106a2c, 0x40de52b1cbf84c4b },
            { 0x4ae58b4188ab912c, 0x266c6d241524d453 },
            { 0xe842f2b6eee0b0a2, 0xff221f1bb63ea43b },
            { 0x56147a54dd123693, 0xb2bd637ad89b274a },
            { 0x71865e15665afdc1, 0x8f82a27374646de4 },
            { 0x637d77fae358f6c7, 0x413c32cd7f872692 }
        },
        {
            { 0xdf0c2add3b567ff7, 0x7397e347a08d8ad0 },
            { 0x97f90e6421d8f7fd, 0xcf224d61d0286c3a },
            { 0x242c3912a64791fd, 0xf653ea5c7590d593 },
            { 0xf2f446ee07d532b9, 0x5c466790b1ae0ad5 },
            { 0x61fd36baeca0951c, 0xd7f4fe540acd508a },
            { 0x3e2e52a1ef93f747, 0x7c15f40eea90a816 }
        },
        {
            { 0x4c4998514790854c, 0xb50a0ad9382cda07 },
            { 0x45306e870eed29bf, 0xe4af75836bd04df3 },
            { 0x98f53e7322e2b9f1, 0xa5ae20da23bb1fd7 },
            { 0x9c9094660a30968e, 0x9eca13b5afb1acd6 },
            { 0xb9ecc3988e5e09bb, 0x3d35ad268e5de315 },
            { 0x3c2f8601de468a44, 0xc84261c7011ff761 }
        },
        {
            { 0x3ad8813da05de972, 0x9571ea7fb7aab447 },
            { 0x5ee3512dd73fd3e7, 0x5e43f77a97d28598 },
            { 0xc7f37c59af29bfb3, 0xcd96a26c86e7c0dd },
            { 0x6c754377ed9f7e06, 0x1951989deeceba65 },
            { 0x9a6ac5ce3205ce89, 0xab1cbab4c49c16ad },
            { 0x835a862d250b0961, 0xf00b847465f5165 }
        },
        {
            { 0x7394ea8e51037737, 0xaaefbada37caf4cb },
            { 0x100a9462cf345343, 0x13eff8b6aa5aa7dd },
            { 0x863a571811cc58df, 0xb4aac04018fc6baa },
            { 0xd726864512df6511, 0xd4cc834fb8badc72 },
            { 0x316c357635cf1643, 0x2b38adf4f70b676a },
            { 0x5a07ef3d70a4b7c2, 0x98197a1b26fc572f }
        },
        {
            { 0x95c3163213f3e19b, 0xc5059d3bb2fcbc65 },
            { 0x85257ae24331891f, 0x3c621f87bc97c42b },
            { 0xafceb6ee5e4b4c81, 0xd163a33821a44018 },
            { 0xc1488112d6a4c7a6, 0xdd8eaf63860d96be },
            { 0xdcf45d08ee34a546, 0xe71fc78aa04c6d8b },
            { 0x29cb1818de676e93, 0x45bf868dd49e2f43 }
        },
        {
            { 0xdc8f04138514cb30, 0x3d1b59dc191b016 },
            { 0x3eb802d88f9f2046, 0x2406d610560dcb57 },
            { 0xe82c2aa040cc7889, 0xa5f691b006217d53 },
            { 0xcebdecdfa6f913c2, 0x57b45fa808352f1d },
            { 0x5b367cf662145377, 0x430c0f580df0bd10 },
            { 0x74dc15c772a669ed, 0x316ebb12c07d9a8 }
        },
        {
            { 0x58d489a33f096b6b, 0xd0c484af4b46370d },
            { 0xea06267b5b814f28, 0x222a5c7d537d6ec3 },
            { 0xbf8d5c3e5bc4e9ee, 0x71f282b806a140e7 },
            { 0x108f6ec3cf5e23b8, 0xc253feea5f832055 },
            { 0xcf950579f7f9237c, 0x90d7bb3d7cd4e13f },
            { 0x7c61c85cfb5c25c3, 0xc23589c3dd28fc8e }
        },
        {
            { 0xa7757c5862607e12, 0x97bac25347a55896 },
            { 0xa8a109ef13b4c0ac, 0xb916c69c89ab034d },
            { 0xefa5cd8221c8408, 0x546cd6dc9d5891cd },
            { 0xdb908d25f2754893, 0x104bc85b2299ebc },
            { 0x7399a516b4f8c8d7, 0xfddd55f15aba5e03 },
            { 0x4eb47feb2da11e66, 0xecb7e9a4c08c584f }
        },
        {
            { 0x2acdb3e8774f6e53, 0x8f9e514834c53d24 },
            { 0x784f66c52b291c05, 0x2d74f91cbaee5bd3 },
            { 0x2186b0727b1bfe8a, 0xf1ab6dfed481323b },
            { 0x8ab776c42b00c069, 0x136da0960f4e4112 },
            { 0x938900422a73fff5, 0x8ed6968d0520874d },
            { 0xead9c7b86f642f7c, 0x56c40560cdc80e47 }
        },
        {
            { 0x605b577af599d7e2, 0xe4a4db3c60522094 },
            { 0xaa2ddce9fa266087, 0x276b5f615c112c3c },
            { 0xe4db5b47c4c35719, 0xacfbed12610dd068 },
            { 0x3996de451792b245, 0x1cce06ee67a7c041 },
            { 0xa152123d9c7ac6ae, 0x964db20539d4968 },
            { 0x473c0c31c1653980, 0xd98a9eb1bebae214 }
        },
        {
            { 0xefab303673a2a305, 0x88ed37f446a7afed },
            { 0x234a3e7d09337050, 0xe3ec1ea0e32ec503 },
            { 0x5de1c7a89d6adcb, 0x9293d7f00b108d56 },
            { 0x6d2ff18adb9b44e6, 0x87de9bdeb7e84e86 },
            { 0x223c294e828a5ced, 0xe8b2c203c57c5eba },
            { 0x7afd4c14421a5158, 0xf9cd617e78e9b34d }
        },
        {
            { 0xfc6fbaaaf903f1c, 0x57894158443c21c8 },
            { 0x14d1fb731772b586, 0x6193a6f50d33436 },
            { 0x1d19790cbcba271c, 0x981a696fc895fd30 },
            { 0x9984bf2cc461a928, 0x7ab51f6e72965d8c },
            { 0x5d78c138e75b73aa, 0x4ad0f73c2845d2b7 },
            { 0x525ed9f6dc1f29f4, 0xa99771fa84250215 }
        },
        {
            { 0x1db8af796be39e2c, 0x1ae464d864c05d6c },
            { 0xb22bbe1bb2008766, 0xf01104a72c9bb561 },
            { 0xefc6840b41d744ab, 0x3537353b01685c7a },
            { 0xaba40d342e6c214f, 0xc409ffbb4df0331e },
            { 0x41c96ddf82c8aebb, 0xe64b129a5aabde07 },
            { 0x4d69dbf7399387f4, 0x9f69b041c57fcb4f }
        },
        {
            { 0x16cc1b525e817549, 0xe2a12f915b35dec3 },
            { 0x112c9409879a1869, 0xe398dc8119d60ec },
            { 0xd94abb3d88293728, 0xa16f876764fc6473 },
            { 0xeebd8da6cabc62b, 0x65382ffb28d815a8 },
            { 0x4ac1ff85c36fb0b1, 0xeb8a465519593f67 },
            { 0x72dc25220b67d34b, 0x93f48705e0a9b603 }
        },
        {
            { 0xb40dd6c57a0fdbfe, 0x4019c61c263c72bf },
            { 0x2c46c5247928520c, 0xf1bccedf58d14b3b },
            { 0xef84f4ad632c3b6a, 0xc585c2031bc36a27 },
            { 0xe17efb12d0f15e0b, 0x20788c674765af30 },
            { 0xec644eff4eff20a1, 0xd63f1f49c2b1e505 },
            { 0xffbc7a6dbb949cee, 0x11a0eded698baeda }
        },
        {
            { 0x84b69477aa8652d7, 0x4cc1a9f42dd2f8ae },
            { 0x64239332f53b351f, 0xd56e8b5d5e689c9a },
            { 0x3ef772137735875d, 0x164e17306348abe },
            { 0xaee8bfc3b1b6e6bc, 0xe774b2040a3d9aa0 },
            { 0xe52ac9ddc9b63647, 0x79ad6ce68417cc33 },
            { 0x93354c79f968c0d, 0x59dbb23f0ae15857 }
        },
        {
            { 0x59f14d7068d59512, 0x45bfe6d3371845b1 },
            { 0x126d91534ae0013f, 0x4621bb9345367166 },
            { 0xc593c1c23b3716bc, 0x436ec008ad399046 },
            { 0xcf9267c74fe60630, 0x7a38d642145f902b },
            { 0x163f7e0c032a4f8b, 0x8943dce1aa67f692 },
            { 0x4d9a47623144ca7, 0xca3486949b8556d3 }
        },
        {
            { 0xd80fad6fd24be00b, 0x5bcf5eb4614f81a9 },
            { 0x689edb6e2242422, 0xfee1599c4e5831cd },
            { 0xf32c1f04bbbb9414, 0x7f5c07d8667b111a },
            { 0x24372ffc4ef94524, 0xfe4b6c536ade02a6 },
            { 0x520984625358f787, 0xba9302349931af10 },
            { 0xba3d6b97af70ff7e, 0x4475c33ad761c7db }
        },
        {
            { 0xb4f3ab327c43e7da, 0x7a2b1cb24f3a5cc0 },
            { 0xedfee75582e268e3, 0xd007f0a78cc9609c },
            { 0x194e062049e37aab, 0xece749e2f475ed84 },
            { 0x6779e0bc2e462f43, 0x462039b71b761553 },
            { 0x347d58b074f23a6a, 0xa0e1baddce368a7 },
            { 0x8779afe5fd6f0340, 0xa7d0b8bc99a46cdc }
        },
        {
            { 0x9067dee1c0956f74, 0xd9a83dc3c194c89d },
            { 0x48ae6d4545b3ce9a, 0x7f8982bbeb0a3ab9 },
            { 0x620628650e38e48f, 0x79086385c70872ce },
            { 0xab1c5b58696b9d60, 0xc6bd34de22f3beb1 },
            { 0x55e9e1a7b3213cb2, 0xf8fcd3eebef73e8 },
            { 0x9998f07696e37dc8, 0x9e48b4eaebccce8c }
        },
        {
            { 0x9c0f3f502a29d270, 0xbaeb2e5e63f73df },
            { 0x6bba910f334d6702, 0xcba1046511778bb9 },
            { 0x7eeed075535551be, 0xdce46a9ec10cd32 },
            { 0x47e481d649b7abbc, 0x722d14d903d09408 },
            { 0x85d00a5067975598, 0xaa46848d16986a97 },
            { 0xf7df1e5cd977cfab, 0x671b25361d469577 }
        },
        {
            { 0xaeb2d7e89572bc83, 0xee4bcfd813cd2c30 },
            { 0xf845da2672f232c0, 0xed48e6ce0180bfd3 },
            { 0xe4b797d6cb2ca8ef, 0x2413a981d45d2378 },
            { 0xbf3974180a602175, 0x68511e81f7b0f105 },
            { 0x52911e5b67619d6d, 0x6baf312e61c2d722 },
            { 0x4174f04fb993fe58, 0x64d5c3dbf5900c3f }
        },
        {
            { 0x9415dbbe99fd8165, 0xcbff5141f5a9f002 },
            { 0xa6d38c904e97ca1a, 0x7096721652b3a0b1 },
            { 0xfd0edaacdeadc98e, 0xdadfbfbcfd7b4224 },
            { 0x7166e534e522d018, 0xc9051dce338dfd72 },
            { 0x436cfb89dce6bb45, 0xce0c54956834eb2e },
            { 0x859793b404478586, 0xf1447cee072c1fa8 }
        },
        {
            { 0x6672627cce7f3abd, 0x516b42cc22b991be },
            { 0x4312fe6b7595fcaa, 0x71c73a9527f46d03 },
            { 0xd5c71da3d2dd1a40, 0xe0f3cda13aaa6a3 },
            { 0x653b962205eada46, 0xeb6b51ddcf2bcf9 },
            { 0x7a9572130e9c6824, 0x167d22d056a7784f },
            { 0xcf3a3f0c873764ab, 0x7f64ed6448d68644 }
        },
        {
            { 0x21a7bba82b2dad56, 0xab9e34f46f0d7767 },
            { 0x525e0f80c519f3aa, 0x5bc15d511e43f22f },
            { 0x7e66e90b8ee5395e, 0x58484813c30c345a },
            { 0x36189a1cda4e38f2, 0x878cf4849b7155a2 },
            { 0x66bded5ef184d583, 0xe7b4d7cc0c7b4528 },
            { 0xf2f78b492d0526be, 0xb351e9dd75948865 }
        },
        {
            { 0x93cddad9a53092c2, 0xa53b5211d4b7762d },
            { 0x5db053cfd3769ae8, 0x30c6bb2e9559190 },
            { 0x11a62a774bc8c10f, 0x64bc25f9f66e046e },
            { 0x3f201eed662322a6, 0xf3ca50f159a13ac0 },
            { 0x97f440fae68e0ca3, 0x3cdb3a18db0cf8c4 },
            { 0xe5d9b761c989a196, 0x5da6ebd423fa61e5 }
        },
        {
            { 0x989ded1b0270cc64, 0x3e669c88e16b1a62 },
            { 0xf9b217c69f541ec2, 0x679c7a8533f8744c },
            { 0xe9452a9a325eb348, 0x578b7a3f5d673f6e },
            { 0x7e28a1b9bd4561b7, 0x1b78d3af9feb5bac },
            { 0xbe6f9d4758203ef3, 0x15c60626aba4eaf6 },
            { 0x4fb34ffc8e9c1ec9, 0x185dd320313f3ac6 }
        },
        {
            { 0x47523cee6736a423, 0xdaeb62c382788637 },
            { 0x4c847b92b720f843, 0x25dbf581b9317caa },
            { 0x64fc9be2ec3d239f, 0x30f794be977e50b6 },
            { 0xbcd383f6b97530fa, 0xea7bfc8070c70d80 },
            { 0x8a82f5b7cc4ed1ba, 0xd6c0818a19676173 },
            { 0x959c90cd31189edf, 0xc041829946f59cc5 }
        },
        {
            { 0xfb13724d03b2b258, 0x5faa44d543eed93b },
            { 0xfd30aaec9b082c9b, 0xf41f6d3cd8d66548 },
            { 0xba714d9f63cfa15f, 0xd99a236ee0a8d348 },
            { 0x209139da64ec9bd8, 0xbacd613893987e7b },
            { 0x77f6c1f37b7c9c4a, 0xe0a9848e3a2d4109 },
            { 0x28c24607013088f2, 0xa231e4f312e4ae2 }
        },
        {
            { 0xbec3fed7f3d435b3, 0x957240f382c931b4 },
            { 0x93b23a4c7524d6e6, 0x24375efe77495d65 },
            { 0xa9a6aea6d7c135f6, 0x2f28682d01dda326 },
            { 0xe80047d52b8dbe21, 0xed2df10bb69213d5 },
            { 0x553a406939535a05, 0x9fede87b6497025c },
            { 0xff89335335ee47a3, 0x459f4cdf54f63841 }
        },
        {
            { 0xd0de1a6553eb24bd, 0x5e656a0f830d85e3 },
            { 0x802b19c730e83669, 0x480076afab0931eb },
            { 0x5ffa42e79a7d9602, 0x4f7b10b3362eb52b },
            { 0x216cf8079ba5fddd, 0xb71ad74dfdcd2640 },
            { 0x2bd2b544e8fc8a93, 0xaf6744219f0431f9 },
            { 0xab7a167c97c4c1d, 0xec2ec2b29c0f044e }
        },
        {
            { 0x80498281ec5d2a36, 0x3ee3324650be170a },
            { 0x7bcf38693a1a503d, 0xc511b9cb40da5f65 },
            { 0xa33d018b628b520d, 0x14eaa5f8002e71da },
            { 0xd80ac767bc8d9c3e, 0x9c6b478bb4b6f195 },
            { 0xc91ad808a925eaf1, 0x1a6ade0c2abec51e },
            { 0x1d22d50bbc0c8940, 0x665c02f995bf1e48 }
        },
        {
            { 0xbf4578aaec663fdf, 0x8f8e1a4ac38380f7 },
            { 0x931a0634a3133eee, 0x9541d6154d752720 },
            { 0xf65f9850419ed889, 0x143c73a506f4025d },
            { 0xa15392d440dc07ca, 0xbcf4b68429a2839a },
            { 0xf3657790bec61703, 0x6f1bc4d735e2f2e2 },
            { 0xafd4a5acff060c37, 0x75d75b6bba794883 }
        },
        {
            { 0xe4a80150a004eab4, 0x5e9e35a1bd58cefa },
            { 0x5e9a423944fc4a78, 0xe398614f6f2d9eb7 },
            { 0x7d9e08bf7233ff99, 0x2779b2667eb0594f },
            { 0xeb5e30e97bbbacbc, 0xea4f70cbbb3d3643 },
            { 0x7fc492a99e89a208, 0x8239d6a1477bfb62 },
            { 0x1491b40dd5d7c74b, 0xe2302c9b25a9987c }
        },
        {
            { 0x51af80886cf7d810, 0x9374c506a51af63c },
            { 0xd7eb835001b1a990, 0x1aa3a167bac5bb0e },
            { 0x4e0b9570533fd92, 0x636de5def0c3b367 },
            { 0xaba0367e29244612, 0x6695bab7c46cef7e },
            { 0xf40d736ca1365520, 0x75e3a32d23d0b7b8 },
            { 0xf7e20e00762666c4, 0x50d3a21c27c4dea6 }
        },
        {
            { 0x3a2596afd2be6da2, 0x7b6fd534deea141b },
            { 0x533b024efdfbad93, 0xd6494b426747aeeb },
            { 0x5d2faabd300dfd64, 0x7cc01c1b20689612 },
            { 0x4470ead1ca85f8ab, 0xfb91dbbe59f65d08 },
            { 0xfedcb95b0e8b238b, 0x75de67b7361f7014 },
            { 0x59d164866d17df0, 0x9dc55def715f65d3 }
        },
        {
            { 0xbdc0909710d9bf47, 0x8f17b79fe560bd53 },
            { 0x90ffba2c6947e714, 0xc50efd3ea73efd88 },
            { 0xcd1b126ad5c0d239, 0x10aa0b3ba51e151 },
            { 0x1b90624133422710, 0x2fcb92f6cb7ae25f },
            { 0x68edde52af8a8c25, 0x31dc935d8ba87691 },
            { 0x8c0f58a5958f18be, 0x548e23a980194639 }
        },
        {
            { 0x908c60dc8860f466, 0x4c10c178e0a702c },
            { 0x6482e23d170b545c, 0x426904a5b6f50926 },
            { 0x2f37ce89f64d971d, 0xb48eca091c1cfcd6 },
            { 0x5f47297b535fd903, 0x4e3420489d8276a0 },
            { 0x410d1bc57828a19d, 0xf842a11382aee7c9 },
            { 0x220f28243892b70d, 0x974ab739fc0f65e2 }
        },
        {
            { 0x72aa7b4ed6cd2e07, 0x8a656cdc0fc2fdaa },
            { 0x8549cd4ef945f4b6, 0x3722e10fa94f2e9 },
            { 0x47ff34f9cf47d600, 0xe928f3839a39e02d },
            { 0xd6d8227520a3e48c, 0x279dd2dd4391b158 },
            { 0x367d91aad9f1ff7e, 0x944c5c93649f16b7 },
            { 0xfdc1431a95d75eba, 0x262b3573a2ab734b }
        },
        {
            { 0x5c0f7b72171ee72c, 0xf0f2289287580e1e },
            { 0x1106c25b3ea82a4a, 0x8ea8a1e07e12ab3f },
            { 0x4025221de14cc0c, 0x8d75f79e291af67f },
            { 0xaa70bd0b7ebf79d6, 0xe6083951ebe06aa6 },
            { 0x7502b4b2a39ed97f, 0xcce71efbe117e37b },
            { 0xff4f1ec2e332ad73, 0x9f21d77b6d06e1af }
        },
        {
            { 0xe4f2eb015ec60625, 0x6e9e453e57976f03 },
            { 0x3f950e3f6bbc9df3, 0x9e5b73ab403facc6 },
            { 0x4f0d3f853d4944c3, 0x9f83244a1d8f9307 },
            { 0xfef56446cfbba39b, 0xf03a5685eb270b19 },
            { 0x43b6e1cf48d75005, 0x355c08918e22523a },
            { 0xd61bd120245bcba5, 0x9b5ce77d74eccb83 }
        },
        {
            { 0xa63dab8b00006a21, 0xfd96eb7a58184c06 },
            { 0x10e16d9b2f71f557, 0xe58aa00521bb9af },
            { 0xbd0462832ad6ddb2, 0x30c14ad4fbc4e3ba },
            { 0x96b832292e19413, 0x8a8a2c04cdbdb7a2 },
            { 0xeb9c1a5ea1e1645e, 0x94df1c939d46ea17 },
            { 0x5e55685ace091814, 0xdf12aa6b1dc736dd }
        },
        {
            { 0xbf8690c2dffb4c8f, 0x2946c5f372c5d510 },
            { 0x2c5333d871f50a07, 0x7d31dbcd4361465e },
            { 0xdfcefa873ebdbcfa, 0xf5d2e41f89e9c447 },
            { 0xdcafaf7a5ae6570a, 0xbd0b0dc035d9d118 },
            { 0x57a732e0fdaf5297, 0x899c264fb85d928e },
            { 0xb64aa88e5c7194b6, 0x11b41b5f76fb0ccc }
        },
        {
            { 0xea94791dd0760052, 0x114ba7252d952e55 },
            { 0xf9b48513586b1c03, 0x58f712f35638fdbd },
            { 0x7bd4ed44a107672c, 0xc2021ba9ddb6b68c },
            { 0xc8d94ec825124039, 0x2be9ce66b1e10b91 },
            { 0x4d03f25b5c4a1e1b, 0x81a8407cc96bc91b },
            { 0x48455f25d755ac1b, 0xbcb67837790862b5 }
        },
        {
            { 0xb336b2c84d301ed1, 0xcde67e64bf4bac74 },
            { 0xfb8d0d18f680386a, 0xfe7e6602922f880c },
            { 0xcac37577b6707ed1, 0x253b32211a61b180 },
            { 0xad5d93f032b5d1ed, 0x56a9684ebf728229 },
            { 0xcad1df9c81d1acc1, 0xa9f3240f45247fa6 },
            { 0xceddcca0a06a9e10, 0xf57b36f61c1e428d }
        },
        {
            { 0xe806df4ef3f01ecb, 0xec940b75f1769d78 },
            { 0xdb7dd99188ae6995, 0x649fa17bbad9be9e },
            { 0xc6131a2385ec0e48, 0x8699c10925785b14 },
            { 0xaf2ea39782fd2775, 0x218d3ead92106176 },
            { 0xa2d1306dac8a76cc, 0xdb13c3eb3b9d34b5 },
            { 0x36c411c0d5afdc23, 0x3ca8fe2b64c94863 }
        },
        {
            { 0x71f12f80bdd9cc27, 0x9c744680cd89f90 },
            { 0xf409fbc739f31cf0, 0x6ceb0e93bc28b1e },
            { 0xaef56bd526ac2781, 0x367d6b38f9213749 },
            { 0x723757a741ce305e, 0x2be87866e492b0f },
            { 0xbbed663ab950133, 0xc8a7fd4379a4f854 },
            { 0x8800c12a88433842, 0x96a1b7167a1e4296 }
        },
        {
            { 0xce977863ef785530, 0xb4300d4f73512d64 },
            { 0xbd2cc6b1e83db36e, 0xe8ce64f9bc3391e6 },
            { 0x497b0003a2381080, 0xa29ca0cbc34ca9b0 },
            { 0x82caa350aba5f30e, 0x15e1f3d500e3fbba },
            { 0x95e8aa32afee74c1, 0x750ef7566a9724b1 },
            { 0xc33657417b2376df, 0xd70c24de58b7c61e }
        },
        {
            { 0xccc23864a0d59a89, 0xda8d2686584f96dd },
            { 0x2688abf3fdc39199, 0xae1fb6b4513a3807 },
            { 0x9bc68d5293e2d502, 0x9eebc5ee3efe3eef },
            { 0xd21281ccf85a48b9, 0xd5ce4b76ad4bb81b },
            { 0xfedbd17828d592e5, 0xc633ee3ea5d47c8f },
            { 0x4e3b976c4cf82b7a, 0x70445b1a9433b432 }
        },
        {
            { 0x544ff4e8dd217d09, 0x64de1a629f521021 },
            { 0xaabd68b38351af7b, 0xfa1f534593960e47 },
            { 0xc5cd3d83832376, 0x3b14a8096f077d08 },
            { 0x9ac57af6315af510, 0x6b45e5d21e39278c },
            { 0xcfaa412b720e08eb, 0x40a95242e367529c },
            { 0xe15eabc0f36fbd9e, 0x59d8256dc6f9f49 }
        },
        {
            { 0x7f2d8bfd065aa5df, 0x4a1bba6017160461 },
            { 0x7e2dc6f89e5de5aa, 0x50c1ce6a64612967 },
            { 0x2e5a6f564ccbb4d7, 0xf7cb1f96aee59f79 },
            { 0x26794b97e92c2adb, 0x543c1773e3d32d0a },
            { 0x6922f57afd1ebd3d, 0xd12ccc0caf691994 },
            { 0x6547361b248a24fc, 0x55674c3a6002db00 }
        },
        {
            { 0xdeaabe94d16f1955, 0xd27e29a5a5cd8051 },
            { 0x2e0b07bc11af86ac, 0xe1e3cfd6485fdc73 },
            { 0x8e3b6358e2b21ff9, 0x916fb83aa116bad0 },
            { 0x345f4deda0aad9, 0xbc192096132af6c3 },
            { 0x95d1553122ccfe16, 0x34a22ba96dd766be },
            { 0xe1479cc93c39b8, 0x7a65ba1b3a8d7680 }
        },
        {
            { 0x2eb75d4082bbda19, 0xd27add9004066317 },
            { 0x798aef6d8dead56f, 0x5ffe39f2b58ab706 },
            { 0x87c3519635fb3cc2, 0xf028ba3139576e94 },
            { 0xc54861b67bf27ab1, 0x6d84620a0b59eaf6 },
            { 0x10009f4e0a516ebd, 0xb833529ce172daaa },
            { 0x19f97ba1746f1861, 0x69f680878f3d8d80 }
        },
        {
            { 0x833a2086677e0665, 0xc5ba67f235a4ca4d },
            { 0xc473fda071b692c3, 0xd2741064c05baf2a },
            { 0xb5bedc075900dba0, 0x8092583ffb6fc07a },
            { 0x55098fd038fa9a0a, 0xace8b62b884e1bc4 },
            { 0x88913856fc92c06f, 0xae195c0efb4e54d3 },
            { 0x449ed889a8475eb6, 0xd8d047bdf3a2d18f }
        },
        {
            { 0x81b4547914262057, 0xa4f3e16e9015fd8f },
            { 0x92fc8ee97c22d434, 0xf8f5d2e7d128f09d },
            { 0xa426b6f3f47c9e77, 0xb83fb40eb6ad9a06 },
            { 0x155e332ca59016dc, 0xaa5639c990b728ea },
            { 0xb7a9fc412609df30, 0xc172c17cfa57ced8 },
            { 0x6788eae0e7f82d75, 0x361467d3fd00330e }
        },
        {
            { 0x6e1d49e7555b7ae5, 0xc9a52a0d9ce6d139 },
            { 0x2517ca81836bb5e9, 0xfe5daf8edb31a276 },
            { 0x147f336c7e903fe8, 0x9de597d1fdd0a341 },
            { 0xe9102830be494381, 0x8c59dfda6bc361b6 },
            { 0x83ecf653cf8ae0fc, 0x95f003b26facd4a1 },
            { 0x46c6e00190ede833, 0x9a560f3d0eb98ab4 }
        },
        {
            { 0x1ec50c62e531acf2, 0xddb1284e7119bd29 },
            { 0x3de83417c100afb1, 0xd6fc6ec678b1347e },
            { 0xf21c534a08784d80, 0x82d2af2480f25077 },
            { 0xa1be0ed1e02dfd78, 0x8ef9daa04b81e101 },
            { 0xdd51bcc4d041ce13, 0x594588c4a6c3da65 },
            { 0x9fd36faa41666679, 0x29002f965162c61f }
        },
        {
            { 0xd6b710489e4ca5, 0xb52de25cc108b82a },
            { 0x8482261968b6f391, 0xd78893898534e8b8 },
            { 0xf8ac249c3c268d8b, 0x8f0dc7ee1f69e6e6 },
            { 0xad40743936baa913, 0x296d7a33a985b373 },
            { 0xae5a8f114902e3d0, 0xf88f00b589bea675 },
            { 0xba24c752aed4fc01, 0xfc427c4dbd16c679 }
        },
        {
            { 0x47b1a4c0a17967af, 0x31bc07a22e3a5806 },
            { 0xb43a8826e0149dba, 0xbf37da630bd57af1 },
            { 0x597c580779d2c53, 0x8b3c81c304366461 },
            { 0x294a3734d6c0a1e, 0x924effaaf294ba60 },
            { 0x3b50239148f4f637, 0x3de8c64672994fa5 },
            { 0xc72dae6cf42ccda, 0xbfad9bede06db580 }
        },
        {
            { 0xbc5e7de07d0f23a, 0xc068ae06863bc844 },
            { 0xae5c0695e8639cd0, 0xcdf8686952f00c5f },
            { 0x4d70face0096be, 0x54c4f782049289cd },
            { 0xb5cde5457598be9, 0xa2b06724ee098483 },
            { 0xc11307142840df90, 0x49d330efb40bf82f },
            { 0xaa6ec3517ec05010, 0x26c9bb92c6e5ea }
        },
        {
            { 0x13489c76d8b491c0, 0xedf300940c39da93 },
            { 0x5b8a37d9b1fa8738, 0x1db6debcd5aac08c },
            { 0x68836d30de00cd3, 0xe878639cc8f19316 },
            { 0xf8d62d9d880c9f30, 0xc1cb7954cfba9b7c },
            { 0x2ce2316b7c50148d, 0x3e0cbd241fdaf643 },
            { 0x36b8bebaa13e6fa4, 0x47320a7e3c800764 }
        },
        {
            { 0xa16b3f888209dd25, 0xa3573b7f7119ffb3 },
            { 0x5a38cfdcf2f1275f, 0x9acce6e6ed40bdda },
            { 0x7e364dc85de6e7c6, 0x17795246044e6612 },
            { 0x361cc7b022151a50, 0x19f3b7254f05d188 },
            { 0x4b9ad79b68f52cd3, 0x9ad3d58dc88b6dd3 },
            { 0x168f4c867d0952c2, 0x1a4b300c2092b525 }
        },
        {
            { 0xe1c7270d8f26215c, 0xb4ee1e5b7c692c07 },
            { 0x6b10057588884a79, 0x1df1abbe267240e1 },
            { 0xd5f94f8ba6e92735, 0x24d984b40cd7f67d },
            { 0xefa814d6e270dde, 0xf11cbb62bbb9267b },
            { 0x48b7c90099620183, 0x138d2d00a8e50b0a },
            { 0x3b58f3ebfd1a5f53, 0xa0a896e00008329a }
        }
    };

    static uint_fast64_t castle_codes[2][2][2][2] = {
    {
        {
            { 0x8a65f79c5ef026b7, 0x4103ce678c58e5e1 },
            { 0x2f37a31cbba3df04, 0xbcd5d414db23f576 }
        },
        {
            { 0x2b4de46e5a1f77ce, 0xa199174aceef50f },
            { 0x35e1ad8b6449e203, 0x3705d7094e5fb6f4 }
        }
    },
    {
        {
            { 0xb3c544a40fa269a9, 0xf4477b4cd6bfb7f5 },
            { 0xa86036699f5535f1, 0xd3a9f262491339ea }
        },
        {
            { 0x39b2824ceee87dc8, 0x7a64f095ce7c0005 },
            { 0xc09551180e312d87, 0xa4181588a376e114 }
        }
    }
    };

    static uint_fast64_t en_passant_codes[8] = {
        0xa55c81df5e71cea9, 0x9326a23571bb093e, 0x41bd144246eb742a, 0x5f2aee73f256a333, 0x6f7ea440c76cc947, 0x424100ddfe314e9b, 0xcfb8d22cb960a203, 0x8ae3366391b4840a
    };

    static uint_fast64_t black_move_code = 0xfc5d6f1414f4caa;

} // namespace Chess::Hashes
