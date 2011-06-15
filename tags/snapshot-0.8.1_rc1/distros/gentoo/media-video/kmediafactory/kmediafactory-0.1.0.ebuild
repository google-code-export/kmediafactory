# Copyright 1999-2004 Gentoo Technologies, Inc.
# Distributed under the terms of the GNU General Public License v2

inherit kde
need-kde 3.3

DESCRIPTION="Template based DVD authoring software"
LICENSE="GPL-2"
HOMEPAGE="http://www.iki.fi/damu/software/kmediafactory/"
SRC_URI="http://www.iki.fi/damu/software/kmediafactory/${P}.tar.bz2"

RESTRICT="nomirror"
IUSE=""
SLOT="0"
KEYWORDS="x86 amd64"

DEPEND="${DEPEND}
    >=media-gfx/imagemagick-6.1.3.2"

RDEPEND="${RDEPEND}
    >=media-video/dvdauthor-0.6.10
    media-sound/toolame
    media-video/mjpegtools"

