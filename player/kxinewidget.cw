<!DOCTYPE CW><CW>
<customwidgets>
    <customwidget>
        <class>KXineWidget</class>
        <header location="local">kxinewidget.h</header>
        <sizehint>
            <width>720</width>
            <height>576</height>
        </sizehint>
        <container>0</container>
        <sizepolicy>
            <hordata>0</hordata>
            <verdata>0</verdata>
        </sizepolicy>
        <pixmap>
            <data format="PNG" length="1188">89504e470d0a1a0a0000000d4948445200000016000000160806000000c4b46c3b0000046b49444154388db595cd6b135d18c57f33b9339986183b11db4493c6850dd1d89285a0048a1f48b5746745dc0815a45d88ff84eebb72e1525410115d885414e3375908929060413a5a9b7ea44d13a8b526997492ebe2c54879abc80bef59de7b39e7720ecf7994743acddfc0b22cb977ef5ee5af1e03ca9f8893c9a4340c836030c8c0c0001f3f7e6479799962b1886ddba4d3e9df0a89df5d9c3c79521e3e7c9883070f128bc50887c394cb656ab51a535353643219060707e5d3a74fb724ff17f1850b17e4e8e8286fdfbea5b7b7978e8e0e72b91c00baaeb36bd72e2e5dba44bd5e677a7a9a870f1fca9b376f72e3c68d4d026d2b8e1c3922c7c6c6b87cf932ddddddd8b68da669dcb97387bb77efd2dbdb8b9412cbb2387ffe3c232323341a0d0cc360656585898909ae5fbfdeb64749a7d39c3871425ebd7a953367ce20a5a4d168a0aa2a524a363636c86432944a255656563870e00089440221042e978b66b389aeebb85c2e6edfbecd952b5748a5528a00b878f122434343148b45161717999999a15aad128fc7e9e9e9a1afaf0f4dd30068b55ad46a35969797c9e572b8dd6e76efde4d281462707090a9a9a97fac884422f2dab56b747676f2f9f367161616f8f2e50b009aa6313434c4fafafaa61c363636c8e7f394cb655455251a8da2eb3aaaaad26ab598989840edefefc7eff7b3baba8ac7e361616101c7711042108bc568369b44a351b66ddb86cfe7c3ebf5128fc789c562783c1e1cc7219fcf238440d334c2e130fbf7ef4798a689c7e34151141445e1dcb973341a0d344dc3300c1cc7c1711c02810000524a6cdb261e8f138d46b16d9b56abc5dada1a524a0281007ebf1fa1280a8661a0280a524a00dc6e773b44c3301042d0dddd8d941297cbd51e908e8e0e344da356aba1aa2a9d9d9d188681aaaa88efdfbfa3eb3a4288b6aff97c9e172f5e502a95a856ab1c3a7488d3a74f2384a052a970ebd62d2a950abaaee3f3f9181f1f67fbf6ed48299152f2eddb37d4f9f979666666300c03afd78bcfe7e3fdfbf7bc7af58a4aa542abd5e2c99327140a0582c120efdebd2393c9b0b4b4442e97637d7d1dcbb2304d13b7db4da150a0582ca2cecdcdf1e8d1232ccb221008609a26a74e9d42d3341e3f7ecc9b376f884422ecdbb70f4551482412341a0d9e3d7b46369b0520168b619a261f3e7c6072729242a18002c8e1e16142a110c78f1fe7ecd9b300349b4d52a9147ebf9ffefe7ea494388e83dbeda65eaf93cd66711c87bebe3e76ecd8412a95e2fefdfbcccece323939f96bf24646465014856030c8f0f030030303bfeba74d989e9ee6debd7b7cfaf4897abdce83070f78fefcb9d2ee8a63c78ec9a3478fb267cf1ebe7efd8ad7eb25140a110e87e9e9e9c1ebf502b0b6b6c6ecec2cf3f3f32c2e2e52ad56f1f97c5896c5cb972f79fdfaf5afaef889643229239108894482aeae2e1cc7a15eafd36c3637fd5208816118b85c2e969696c866b3cccdcd6deae72d8b3e994c4ad334e9eaea62e7ce9dedc4016cdba652a9502e9729954aacaeae6e59f87fdc203f45b63affd3f6f82be2ff0af57f61057e0084e7fefb8a884e330000000049454e44ae426082</data>
        </pixmap>
        <signal>signalXineFatal(const QString&amp;)</signal>
        <signal>signalXineError(const QString&amp;)</signal>
        <signal>signalXineMessage(const QString&amp;)</signal>
        <signal>signalXineStatus(const QString&amp;)</signal>
        <signal>signalXineReady()</signal>
        <signal>signalXinePlaying()</signal>
        <signal>signalHasChapters(bool)</signal>
        <signal>signalPlaybackFinished()</signal>
        <signal>signalNewChannels(const QStringList&amp;, const QStringList&amp;, int, int)</signal>
        <signal>signalNewPosition(int, const QTime&amp;)</signal>
        <signal>signalTitleChanged()</signal>
        <signal>signalVideoSizeChanged()</signal>
        <signal>signalLengthChanged()</signal>
        <signal>signalRightClick(const QPoint&amp;)</signal>
        <slot access="public">polish()</slot>
        <slot access="public">slotPlay()</slot>
        <slot access="public">slotStop()</slot>
        <slot access="public">slotSetVolume(int)</slot>
        <slot access="public">slotToggleMute()</slot>
        <slot access="public">slotSpeedPause()</slot>
        <slot access="public">slotSpeedNormal()</slot>
        <slot access="public">slotSpeedFaster()</slot>
        <slot access="public">slotSpeedSlower()</slot>
        <slot access="public">slotSetVisualPlugin(const QString&amp;)</slot>
        <slot access="public">slotSetAudioChannel(int)</slot>
        <slot access="public">slotSetSubtitleChannel(int)</slot>
        <slot access="public">slotSetFileSubtitles(QString)</slot>
        <slot access="public">slotStartSeeking()</slot>
        <slot access="public">slotSeekToPosition(int pos)</slot>
        <slot access="public">slotSeekToTime(const QTime&amp;)</slot>
        <slot access="public">slotStopSeeking()</slot>
        <slot access="public">slotEnableVideoFilters(bool)</slot>
        <slot access="public">slotEnableAudioFilters(bool)</slot>
        <slot access="public">slotEject()</slot>
        <slot access="public">slotEnableAutoresize(bool)</slot>
        <slot access="public">slotAspectRatioAuto()</slot>
        <slot access="public">slotAspectRatio4_3()</slot>
        <slot access="public">slotAspectRatioAnamorphic()</slot>
        <slot access="public">slotAspectRatioSquare()</slot>
        <slot access="public">slotAspectRatioDVB()</slot>
        <slot access="public">slotZoomIn()</slot>
        <slot access="public">slotZoomOut()</slot>
        <slot access="public">slotZoomOff()</slot>
        <slot access="public">slotToggleDeinterlace()</slot>
        <slot access="public">slotSetDeinterlaceConfig(const QString&amp;)</slot>
        <slot access="public">slotGetInfoDelayed()</slot>
        <slot access="public">slotSetAudiocdDevice(const QString&amp;)</slot>
        <slot access="public">slotSetVcdDevice(const QString&amp;)</slot>
        <slot access="public">slotSetDvdDevice(const QString&amp;)</slot>
        <slot access="public">slotSetHue(int)</slot>
        <slot access="public">slotSetSaturation(int)</slot>
        <slot access="public">slotSetContrast(int)</slot>
        <slot access="public">slotSetBrightness(int)</slot>
        <slot access="public">slotSetAVOffset(int)</slot>
        <slot access="public">slotSetSpuOffset(int)</slot>
        <slot access="public">slotSetEq30(int)</slot>
        <slot access="public">slotSetEq60(int)</slot>
        <slot access="public">slotSetEq125(int)</slot>
        <slot access="public">slotSetEq250(int)</slot>
        <slot access="public">slotSetEq500(int)</slot>
        <slot access="public">slotSetEq1k(int)</slot>
        <slot access="public">slotSetEq2k(int)</slot>
        <slot access="public">slotSetEq4k(int)</slot>
        <slot access="public">slotSetEq8k(int)</slot>
        <slot access="public">slotSetEq16k(int)</slot>
        <slot access="public">slotSetVolumeGain(bool)</slot>
        <slot access="public">slotMenuToggle()</slot>
        <slot access="public">slotMenuTitle()</slot>
        <slot access="public">slotMenuRoot()</slot>
        <slot access="public">slotMenuSubpicture()</slot>
        <slot access="public">slotMenuAudio()</slot>
        <slot access="public">slotMenuAngle()</slot>
        <slot access="public">slotMenuPart()</slot>
        <slot access="public">slotDVDMenuLeft()</slot>
        <slot access="public">slotDVDMenuRight()</slot>
        <slot access="public">slotDVDMenuUp()</slot>
        <slot access="public">slotDVDMenuDown()</slot>
        <slot access="public">slotDVDMenuSelect()</slot>
        <slot access="public">slotCreateVideoFilter(const QString&amp;, QWidget*)</slot>
        <slot access="public">slotCreateAudioFilter(const QString&amp;, QWidget*)</slot>
        <slot access="public">slotDeleteVideoFilter(PostFilter*)</slot>
        <slot access="public">slotDeleteAudioFilter(PostFilter*)</slot>
        <slot access="public">slotRemoveAllVideoFilters()</slot>
        <slot access="public">slotRemoveAllAudioFilters()</slot>
        <slot access="protected">slotSendPosition()</slot>
        <slot access="protected">slotEmitLengthInfo()</slot>
        <slot access="protected">slotHideMouse()</slot>
        <slot access="protected">slotOSDHide()</slot>
        <slot access="protected">slotNoRecentMessage()</slot>
    </customwidget>
    <customwidget>
        <class>PositionSlider</class>
        <header location="local">mywidget.h</header>
        <sizehint>
            <width>-1</width>
            <height>-1</height>
        </sizehint>
        <container>0</container>
        <sizepolicy>
            <hordata>5</hordata>
            <verdata>5</verdata>
        </sizepolicy>
        <pixmap>
            <data format="PNG" length="1287">89504e470d0a1a0a0000000d4948445200000016000000160806000000c4b46c3b000004ce49444154388db555dd6b5377187ecec9f9ea214973e29a0f494c9d9846536bc08eb28d6ca2db6809bbd08ae885b0dde884c9fe0715640c3ac664f4d60f10117b55dbb9b58b5dd9868e922f5aa7a6d52669d326395b9aa4c93927e7e4b78bd9820e3f2eb6077e172fbcefc3cbf3e3795e1042f03aeff2e5cbfb5eb79710020a2f07270882c3ed763bc2e1f0fb0f1e3c48afaeaee6f2f9fcaaaaaa4500cd170dbe90d862b1f882c160a4b7b737180804b67bbd5e4fa9545a6f341a7fcecdcdcdc762b1b9542a355aabd572af58ee1f7475751dbc70e1c24f8944a251afd70921444b24127a229130c6c7c7f54422a111428c46a34192c964fdfcf9f3dffbfdfef75eb831cbb2ed274f9efcfacc99339f3a9d4ea8aa0a966571eddab5dfaf5fbf3eba73e7ce1d8410a4d3e9f913274e1c1a1c1c0c699a064110502c16313434343c3c3cfc05006d935814c58e73e7cedd3a72e4c85b8410689a069aa6410841b3d9442c1643a15040b1584477773742a110188681c964826118e0380e26930957af5e9d3a7bf6eca146a3f19709004e9f3e7de9f8f1e31fcab28c870f1fe2eeddbb88c56220844014456cddba155d5d5dd8bb772f9c4e27144541369b45341ac5c2c202aad5ea868c9d954ac5934c2647289fcff7cec58b177fb1d96c585858c0d2d2129e3c79b2210f06060650abd59ed1afd96c22954aa1542a81a669f8fd7e701c079aa6d16ab55a4343437d744f4fcf4776bb1de57219a228aa4b4b4b555dd7c1300c0281000cc380dfef87c56281d56a85d96c46301844201080288ad0751da9546a99619832cbb2f07abdf4eeddbb3f602449ea104511144581a228fed8b1639ca669605916822040d775e8ba0e97cb05002084405555048341f8fd7ea8aa8a56abe5aa542a1421042e970b76bbfd0d86a228451004501405420800503ccf63e313054100c330703a9d2084c06432219fcf435555b4b5b5816559341a0d9aa669d86c360882009aa655667d7d9de7380e0cc36cea9a4aa5108d4651281450afd7d1d7d787c3870f836118c8b28c2b57ae409665701c07abd58a53a74ea1bdbd7dd3ced56a95a373b95cfdf1e3c710040166b31956ab15333333989a9a822ccb68b55ab87dfb3632990cdc6e37eeddbb87582c86959515249349d46a35a4d3694892049ee791c96490cfe71b74369bfd75747414e9741a2e970b9224a1bfbfdf6059f69bf1f1f12fa7a7a7effb7cbed6ae5dbb40511442a110344d2b4c4c4c7c158fc7fb01fc100804204912666767313636864c26f31b05a02d1289fce8f178de3d70e0008e1e3d0a00300ca330393999b4dbeddb7b7a7ade248450baae83e779288a528fc7e333baaeeb7bf6ece9ddb2658b65727212376fdec4e2e262746c6c6c60c379bd838383b7288a72b8dd6e44221184c3e1d7ca96478f1ee1c68d1b989f9f87a228d99191918f1545496c6605cff3fbf6efdfff6d6767e7db6b6b6b309bcdf0783cf07abdd8b66d1bcc663300a052a960717111b95c0ecbcbcba8d7ebb05aad48a7d33fdfb973e7335dd7ef3f13424f61f3f97c9f8742a14f1c0ec70e5dd7a1280a0cc378a689611808820093c9849595953fe2f1f8a56c36fb1d80cabfd2ed39b824493ae87038c21d1d1ddd922479789eb70020aaaa566559ce944aa5d942a1305d2e972700149f2778d505018036005600e2d3ba0e600d80f2b221eaa9dbfe73d0ff0b2b80bf0162446bbaf81f6cf10000000049454e44ae426082</data>
        </pixmap>
        <signal>signalStartSeeking()</signal>
        <signal>signalStopSeeking()</signal>
    </customwidget>
</customwidgets>
</CW>
