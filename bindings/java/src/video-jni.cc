/*****************************************************************************
 * video-jni.cc: JNI native video functions for VLC Java Bindings
 *****************************************************************************
 * Copyright (C) 1998-2006 the VideoLAN team
 *
 * Authors: Filippo Carone <filippo@carone.org>
 *
 *
 * $Id $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/* These are a must*/
#include <jni.h>

#include <vlc/libvlc.h>

/* JVLC internal imports, generated by gcjh */
#include "../includes/Video.h"

#include "utils.h"
#include <stdio.h>

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1toggleFullscreen (JNIEnv *env, jobject _this)
{
    INIT_FUNCTION ;

    GET_INPUT_THREAD ;
    
    libvlc_toggle_fullscreen( input, exception );

    CHECK_EXCEPTION_FREE ;
}

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1setFullscreen (JNIEnv *env, jobject _this, jboolean value) 
{
    INIT_FUNCTION ;

    GET_INPUT_THREAD ;

    libvlc_set_fullscreen( input, value, exception );

    CHECK_EXCEPTION_FREE ;
}

JNIEXPORT jboolean JNICALL Java_org_videolan_jvlc_Video__1getFullscreen (JNIEnv *env, jobject _this) 
{
    INIT_FUNCTION;
    int res = 0;

    GET_INPUT_THREAD ;

    res = libvlc_get_fullscreen( input, exception );

    CHECK_EXCEPTION_FREE ;
    
    return res;
}

JNIEXPORT jint JNICALL Java_org_videolan_jvlc_Video__1getHeight (JNIEnv *env, jobject _this) 
{
    INIT_FUNCTION;
    int res = 0;

    GET_INPUT_THREAD ;

    res = libvlc_video_get_height( input, exception );

    CHECK_EXCEPTION_FREE ;

    return res;
}

JNIEXPORT jint JNICALL Java_org_videolan_jvlc_Video__1getWidth (JNIEnv *env, jobject _this) 
{
    INIT_FUNCTION;
    int res = 0;

    GET_INPUT_THREAD ;

    res = libvlc_video_get_width( input, exception );

    CHECK_EXCEPTION_FREE ;

    return res;
}

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1getSnapshot (JNIEnv *env, jobject _this, jstring filepath) 
{
    INIT_FUNCTION;
    
    const char* psz_filepath  = env->GetStringUTFChars( filepath, 0 );

    GET_INPUT_THREAD ;

    libvlc_video_take_snapshot( input, (char *) psz_filepath, exception );

    CHECK_EXCEPTION_FREE ;
    
    if (psz_filepath != NULL) {
        env->ReleaseStringUTFChars( filepath, psz_filepath );
    }
    
}

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1destroyVideo (JNIEnv *env, jobject _this) 
{
    INIT_FUNCTION ;
     
    GET_INPUT_THREAD ;

    libvlc_video_destroy( input, exception );

    CHECK_EXCEPTION_FREE;
}

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1reparent (JNIEnv *env, jobject _this, jobject canvas) 
{
    INIT_FUNCTION ;
    
    GET_INPUT_THREAD ;

    libvlc_drawable_t drawable;

    JAWT awt;
    JAWT_DrawingSurface* ds;
    JAWT_DrawingSurfaceInfo* dsi;
#ifdef WIN32
    JAWT_Win32DrawingSurfaceInfo* dsi_win;
#else
    JAWT_X11DrawingSurfaceInfo* dsi_x11;
    GC gc;  
#endif    
    jint lock;
    
    /* Get the AWT */
    awt.version = JAWT_VERSION_1_3;
    if (JAWT_GetAWT(env, &awt) == JNI_FALSE) {
        printf("AWT Not found\n");
        return;
    }

    /* Get the drawing surface */
    ds = awt.GetDrawingSurface(env, canvas);
    if (ds == NULL) {
        printf("NULL drawing surface\n");
        return;
    }

    /* Lock the drawing surface */
    lock = ds->Lock(ds);
    if((lock & JAWT_LOCK_ERROR) != 0) {
        printf("Error locking surface\n");
        awt.FreeDrawingSurface(ds);
        return;
    }

    /* Get the drawing surface info */
    dsi = ds->GetDrawingSurfaceInfo(ds);
    if (dsi == NULL) {
        printf("Error getting surface info\n");
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return;
    }

    
#ifdef WIN32
    /* Get the platform-specific drawing info */
    dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
    drawable = reinterpret_cast<int>(dsi_win->hwnd);

    libvlc_video_set_parent((libvlc_instance_t *) instance, drawable, exception );

    CHECK_EXCEPTION_FREE ;
    
#else // UNIX
    /* Get the platform-specific drawing info */

    dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;

    /* Now paint */
    gc = XCreateGC(dsi_x11->display, dsi_x11->drawable, 0, 0);
    XSetBackground(dsi_x11->display, gc, 0);

    /* and reparent */
    drawable = dsi_x11->drawable;
    libvlc_video_set_parent( (libvlc_instance_t *) instance, drawable, exception );

    CHECK_EXCEPTION_FREE ;

    XFreeGC(dsi_x11->display, gc);

#endif
  /* Free the drawing surface info */
  ds->FreeDrawingSurfaceInfo(dsi);

  /* Unlock the drawing surface */
  ds->Unlock(ds);

  /* Free the drawing surface */
  awt.FreeDrawingSurface(ds);
}

JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1paint (JNIEnv *env, jobject _this, jobject canvas, jobject graphics)
{
    INIT_FUNCTION ;
    
    libvlc_drawable_t drawable;

    JAWT awt;
    JAWT_DrawingSurface* ds;
    JAWT_DrawingSurfaceInfo* dsi;
#ifdef WIN32
    JAWT_Win32DrawingSurfaceInfo* dsi_win;
#else
    JAWT_X11DrawingSurfaceInfo* dsi_x11;
    GC gc;  
#endif    
    jint lock;
    
    /* Get the AWT */
    awt.version = JAWT_VERSION_1_3;
    if (JAWT_GetAWT(env, &awt) == JNI_FALSE) {
        printf("AWT Not found\n");
        return;
    }

    /* Get the drawing surface */
    ds = awt.GetDrawingSurface(env, canvas);
    if (ds == NULL) {
        printf("NULL drawing surface\n");
        return;
    }

    /* Lock the drawing surface */
    lock = ds->Lock(ds);
    if((lock & JAWT_LOCK_ERROR) != 0) {
        printf("Error locking surface\n");
        awt.FreeDrawingSurface(ds);
        return;
    }

    /* Get the drawing surface info */
    dsi = ds->GetDrawingSurfaceInfo(ds);
    if (dsi == NULL) {
        printf("Error getting surface info\n");
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return;
    }

    
#ifdef WIN32
    /* Get the platform-specific drawing info */
    dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
    drawable = reinterpret_cast<int>(dsi_win->hwnd);

    libvlc_video_set_parent( (libvlc_instance_t *) instance, drawable, exception );

    CHECK_EXCEPTION_FREE ;
    
#else // UNIX
    /* Get the platform-specific drawing info */

    dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;

    /* Now paint */
    gc = XCreateGC(dsi_x11->display, dsi_x11->drawable, 0, 0);
    XSetBackground(dsi_x11->display, gc, 0);

    /* and reparent */
    drawable = dsi_x11->drawable;
    libvlc_video_set_parent( (libvlc_instance_t *) instance, drawable, exception );

    CHECK_EXCEPTION_FREE ;

    XFreeGC(dsi_x11->display, gc);

#endif
  /* Free the drawing surface info */
  ds->FreeDrawingSurfaceInfo(dsi);

  /* Unlock the drawing surface */
  ds->Unlock(ds);

  /* Free the drawing surface */
  awt.FreeDrawingSurface(ds);
}



JNIEXPORT void JNICALL Java_org_videolan_jvlc_Video__1setSize (JNIEnv *env, jobject _this, jint width, jint height) 
{
    INIT_FUNCTION ;

    GET_INPUT_THREAD ;
    
    libvlc_video_resize( input, width, height, exception );

    CHECK_EXCEPTION_FREE ;
}
