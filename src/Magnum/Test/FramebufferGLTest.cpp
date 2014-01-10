/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Magnum/BufferImage.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Image.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class FramebufferGLTest: public AbstractOpenGLTester {
    public:
        explicit FramebufferGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();

        void attachRenderbuffer();
        void attachRenderbufferMultisample();

        #ifndef MAGNUM_TARGET_GLES
        void attachTexture1D();
        #endif
        void attachTexture2D();
        void attachTexture3D();
        #ifndef MAGNUM_TARGET_GLES
        void attachTexture1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void attachTexture2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void attachTexture2DMultisample();
        void attachTexture2DMultisampleArray();
        void attachRectangleTexture();
        #endif
        void attachCubeMapTexture();
        #ifndef MAGNUM_TARGET_GLES
        void attachCubeMapTextureArray();
        #endif

        void multipleColorOutputs();

        void clear();
        void invalidate();
        void invalidateSub();
        void read();
        #ifndef MAGNUM_TARGET_GLES2
        void readBuffer();
        #endif
        void blit();
};

FramebufferGLTest::FramebufferGLTest() {
    addTests({&FramebufferGLTest::construct,
              &FramebufferGLTest::constructCopy,
              &FramebufferGLTest::constructMove,

              &FramebufferGLTest::label,

              &FramebufferGLTest::attachRenderbuffer,
              &FramebufferGLTest::attachRenderbufferMultisample,

              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachTexture1D,
              #endif
              &FramebufferGLTest::attachTexture2D,
              &FramebufferGLTest::attachTexture3D,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachTexture1DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::attachTexture2DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachTexture2DMultisample,
              &FramebufferGLTest::attachTexture2DMultisampleArray,
              &FramebufferGLTest::attachRectangleTexture,
              #endif
              &FramebufferGLTest::attachCubeMapTexture,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachCubeMapTextureArray,
              #endif

              &FramebufferGLTest::multipleColorOutputs,

              &FramebufferGLTest::clear,
              &FramebufferGLTest::invalidate,
              &FramebufferGLTest::invalidateSub,
              &FramebufferGLTest::read,
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::readBuffer,
              #endif
              &FramebufferGLTest::blit});
}

void FramebufferGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    {
        const Framebuffer framebuffer({{32, 16}, {128, 256}});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(framebuffer.id() > 0);
        CORRADE_COMPARE(framebuffer.viewport(), Range2Di({32, 16}, {128, 256}));
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Framebuffer, const Framebuffer&>{}));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Framebuffer, const Framebuffer&>{}));
    #endif
}

void FramebufferGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer a({{32, 16}, {128, 256}});
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Framebuffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.viewport(), Range2Di({32, 16}, {128, 256}));

    Framebuffer c({{128, 256}, {32, 16}});
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.viewport(), Range2Di({32, 16}, {128, 256}));
}

void FramebufferGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Framebuffer framebuffer({{}, Vector2i(32)});

    CORRADE_COMPARE(framebuffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.setLabel("MyFramebuffer");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(framebuffer.label(), "MyFramebuffer");
}

void FramebufferGLTest::attachRenderbuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachRenderbufferMultisample() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>())
        CORRADE_SKIP("Required extension is not available.");
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::OES::packed_depth_stencil);
    #endif

    Renderbuffer depthStencil;
    depthStencil.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               #ifndef MAGNUM_TARGET_GLES2
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);
               #else
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
               #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachTexture1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));

    Texture1D color;
    color.setStorage(1, TextureFormat::RGBA8, 128);

    Texture1D depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, 128);

    Framebuffer framebuffer({{}, {128, 1}});
    framebuffer.attachTexture1D(Framebuffer::ColorAttachment(0), color, 0)
               .attachTexture1D(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::attachTexture2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    Framebuffer framebuffer({{}, Vector2i(128)});

    MAGNUM_VERIFY_NO_ERROR();

    Texture2D color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(1, TextureFormat::RGBA, Vector2i(128));
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), color, 0);

    MAGNUM_VERIFY_NO_ERROR();

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif

        Texture2D depthStencil;
        #ifndef MAGNUM_TARGET_GLES2
        depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i(128));
        framebuffer.attachTexture2D(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);
        #else
        depthStencil.setStorage(1, TextureFormat::DepthStencil, Vector2i(128));
        framebuffer.attachTexture2D(Framebuffer::BufferAttachment::Depth, depthStencil, 0)
                   .attachTexture2D(Framebuffer::BufferAttachment::Stencil, depthStencil, 0);
        #endif
    }

    #ifdef MAGNUM_TARGET_GLES2
    else if(Context::current()->isExtensionSupported<Extensions::GL::OES::depth_texture>()) {
        Debug() << "Using" << Extensions::GL::OES::depth_texture::string();

        Texture2D depth;
        depth.setStorage(1, TextureFormat::DepthComponent16, Vector2i(128));
        framebuffer.attachTexture2D(Framebuffer::BufferAttachment::Depth, depth, 0);
    }
    #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachTexture3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not available."));
    #else
    CORRADE_SKIP("Not properly implemented yet.");
    #endif

    Texture3D color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector3i(128));
    #else
    color.setStorage(1, TextureFormat::RGBA4, Vector3i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture3D(Framebuffer::ColorAttachment(0), color, 0, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachTexture1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));

    Texture2D color(Texture2D::Target::Texture1DArray);
    color.setStorage(1, TextureFormat::RGBA8, {128, 8});

    Texture2D depthStencil(Texture2D::Target::Texture1DArray);
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 8});

    Framebuffer framebuffer({{}, {128, 1}});
    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), color, 0)
               .attachTexture2D(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    CORRADE_EXPECT_FAIL("Not properly implemented yet.");
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::attachTexture2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #else
    CORRADE_SKIP("Not properly implemented yet.");
    #endif

    Texture3D color(Texture3D::Target::Texture2DArray);
    color.setStorage(1, TextureFormat::RGBA8, {128, 128, 8});

    Texture3D depthStencil(Texture3D::Target::Texture2DArray);
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 128, 8});

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture3D(Framebuffer::ColorAttachment(0), color, 0, 0)
               .attachTexture3D(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0, 0);

    CORRADE_EXPECT_FAIL("Not properly implemented yet.");
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachTexture2DMultisample() {
    CORRADE_SKIP("Multisample textures are not implemented yet.");
}

void FramebufferGLTest::attachTexture2DMultisampleArray() {
    CORRADE_SKIP("Multisample textures are not implemented yet.");
}

void FramebufferGLTest::attachRectangleTexture() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not available."));

    Texture2D color(Texture2D::Target::Rectangle);
    color.setStorage(1, TextureFormat::RGBA8, Vector2i(128));

    Texture2D depthStencil(Texture2D::Target::Rectangle);
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), color, 0)
               .attachTexture2D(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::attachCubeMapTexture() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});

    CubeMapTexture color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(1, TextureFormat::RGBA, Vector2i(128));
    #endif
    framebuffer.attachCubeMapTexture(Framebuffer::ColorAttachment(0), color, CubeMapTexture::Coordinate::NegativeZ, 0);

    CubeMapTexture depthStencil;

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, CubeMapTexture::Coordinate::NegativeZ, 0);
        #else
        depthStencil.setStorage(1, TextureFormat::DepthStencil, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::Depth, depthStencil, CubeMapTexture::Coordinate::NegativeZ, 0)
                   .attachCubeMapTexture(Framebuffer::BufferAttachment::Stencil, depthStencil, CubeMapTexture::Coordinate::NegativeZ, 0);
        #endif
    }

    #ifdef MAGNUM_TARGET_GLES2
    else if(Context::current()->isExtensionSupported<Extensions::GL::OES::depth_texture>()) {
        Debug() << "Using" << Extensions::GL::OES::depth_texture::string();

        depthStencil.setStorage(1, TextureFormat::DepthComponent16, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::Depth, depthStencil, CubeMapTexture::Coordinate::NegativeZ, 0);
    }
    #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachCubeMapTextureArray() {
    CORRADE_SKIP("Not implemented yet.");
}
#endif

void FramebufferGLTest::multipleColorOutputs() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::draw_buffers>())
        CORRADE_SKIP(Extensions::GL::NV::draw_buffers::string() + std::string(" is not available."));
    #endif

    Texture2D color1;
    #ifndef MAGNUM_TARGET_GLES2
    color1.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color1.setStorage(1, TextureFormat::RGBA, Vector2i(128));
    #endif

    Texture2D color2;
    #ifndef MAGNUM_TARGET_GLES2
    color2.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color2.setStorage(1, TextureFormat::RGBA, Vector2i(128));
    #endif

    Renderbuffer depth;
    depth.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), color1, 0)
               .attachTexture2D(Framebuffer::ColorAttachment(1), color2, 0)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depth)
               .mapForDraw({{0, Framebuffer::ColorAttachment(1)},
                            {1, Framebuffer::ColorAttachment(0)}});

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::NV::read_buffer>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::NV::read_buffer::string();
        #endif
        framebuffer.mapForRead(Framebuffer::ColorAttachment(1));
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::clear() {
    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::invalidate() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>())
        CORRADE_SKIP(Extensions::GL::ARB::invalidate_subdata::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::discard_framebuffer>())
        CORRADE_SKIP(Extensions::GL::EXT::discard_framebuffer::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    Renderbuffer stencil;
    stencil.setStorage(RenderbufferFormat::StencilIndex8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, stencil);

    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.invalidate({Framebuffer::InvalidationAttachment::Depth, Framebuffer::ColorAttachment(0)});

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::invalidateSub() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>())
        CORRADE_SKIP(Extensions::GL::ARB::invalidate_subdata::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::discard_framebuffer>())
        CORRADE_SKIP(Extensions::GL::EXT::discard_framebuffer::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    Renderbuffer depth;
    depth.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depth);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);

    framebuffer.invalidate({Framebuffer::InvalidationAttachment::Depth, Framebuffer::ColorAttachment(0)},
                           {{32, 16}, {79, 64}});

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::read() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);

    Renderer::setClearColor(Math::normalize<Color4>(Color4ub(128, 64, 32, 17)));
    Renderer::setClearDepth(Math::normalize<Float, UnsignedShort>(48352));
    Renderer::setClearStencil(67);
    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    Image2D colorImage(ColorFormat::RGBA, ColorType::UnsignedByte);
    framebuffer.read({16, 8}, {8, 16}, colorImage);
    CORRADE_COMPARE(colorImage.size(), Vector2i(8, 16));

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(colorImage.data<Color4ub>()[0], Color4ub(128, 64, 32, 17));

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::NV::read_depth>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth::string();
        #endif

        Image2D depthImage(ColorFormat::DepthComponent, ColorType::UnsignedShort);
        framebuffer.read({}, Vector2i(1), depthImage);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(depthImage.data<UnsignedShort>()[0], 48352);
    }

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::NV::read_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_stencil::string();
        #endif

        Image2D stencilImage(ColorFormat::StencilIndex, ColorType::UnsignedByte);
        framebuffer.read({}, Vector2i(1), stencilImage);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(stencilImage.data<UnsignedByte>()[0], 67);
    }

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::NV::read_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth_stencil::string();
        #endif

        Image2D depthStencilImage(ColorFormat::DepthStencil, ColorType::UnsignedInt248);
        framebuffer.read({}, Vector2i(1), depthStencilImage);

        MAGNUM_VERIFY_NO_ERROR();
        /** @todo This will probably fail on different systems */
        CORRADE_COMPARE(depthStencilImage.data<UnsignedInt>()[0] >> 8, 12378300);
        CORRADE_COMPARE(depthStencilImage.data<UnsignedByte>()[0], 67);
    }
}

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::readBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));

    Renderbuffer depthStencil;
    depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);

    Renderer::setClearColor(Math::normalize<Color4>(Color4ub(128, 64, 32, 17)));
    Renderer::setClearDepth(Math::normalize<Float, UnsignedShort>(48352));
    Renderer::setClearStencil(67);
    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    BufferImage2D colorImage(ColorFormat::RGBA, ColorType::UnsignedByte);
    framebuffer.read({16, 8}, {8, 16}, colorImage, BufferUsage::StaticRead);
    CORRADE_COMPARE(colorImage.size(), Vector2i(8, 16));

    MAGNUM_VERIFY_NO_ERROR();
    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    const auto colorData = colorImage.buffer().data<Color4ub>();
    CORRADE_COMPARE(colorData.size(), 8*16);
    CORRADE_COMPARE(colorData[0], Color4ub(128, 64, 32, 17));
    #endif
}
#endif

void FramebufferGLTest::blit() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::framebuffer_blit>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>())
        CORRADE_SKIP("Required extension is not available.");
    #endif

    Renderbuffer colorA, colorB;
    #ifndef MAGNUM_TARGET_GLES2
    colorA.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    colorB.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    colorA.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    colorB.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    Framebuffer a({{}, Vector2i(128)}), b({{}, Vector2i(128)});;
    a.attachRenderbuffer(Framebuffer::ColorAttachment(0), colorA);
    b.attachRenderbuffer(Framebuffer::ColorAttachment(0), colorB);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(a.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);
    CORRADE_COMPARE(b.checkStatus(FramebufferTarget::ReadDraw), Framebuffer::Status::Complete);

    /* Clear first with some color and second with another */
    Renderer::setClearColor(Math::normalize<Color4>(Color4ub(128, 64, 32, 17)));
    a.clear(FramebufferClear::Color);
    Renderer::setClearColor({});
    b.clear(FramebufferClear::Color);

    /* The framebuffer should be black before */
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    b.read({}, Vector2i(1), image);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.data<Color4ub>()[0], Color4ub());

    /* And have given color after */
    Framebuffer::blit(a, b, a.viewport(), FramebufferBlit::ColorBuffer);
    b.read({}, Vector2i(1), image);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(image.data<Color4ub>()[0], Color4ub(128, 64, 32, 17));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::FramebufferGLTest)