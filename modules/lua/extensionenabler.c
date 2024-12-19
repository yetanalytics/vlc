#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_interface.h>
#include <vlc_playlist.h>
#include <vlc_modules.h>

#include "vlc.h"
#include "extension.h"
#include "extensionenabler.h"

typedef struct
{
    vlc_object_t *p_this;
    extension_t *p_ext;
    MUTEX_TYPE lock;
} extension_activation_t;

#define TIMEOUT 10


static void extensions_GetMenu(vlc_object_t *p_this, 
                    extensions_manager_t *p_mgr, 
                    extension_t *p_ext, ...) {
    msg_Dbg(p_this, "EXTENSION_GET_MENU called.");
    va_list args;

    // Initialize the va_list
    va_start(args, p_ext);
    
    p_mgr->pf_control(p_mgr,EXTENSION_GET_MENU,p_ext,args);

    va_end(args);
    
}

#ifdef _WIN32
static THREAD_FUNC CheckPlayerInitialization(LPVOID data) {
#else
static THREAD_FUNC CheckPlayerInitialization(void *data) {
#endif
    extension_activation_t *init_ctx = (extension_activation_t *)data;
    struct lua_extension *sys = init_ctx->p_ext->p_sys;
    extensions_manager_t *mgr = sys->p_mgr;
    time_t start_time = time(NULL);

    msg_Dbg(init_ctx->p_this, "called CheckPlayerInit on it's own thread");
    int iter = 0;
    while(true) {
        iter++;
        THREAD_SLEEP(5000); // try every 5 seconds
        msg_Dbg(init_ctx->p_this, "checking player iteration %d...\n", iter);
        MUTEX_LOCK(init_ctx->lock);

        // setup
        intf_thread_t *intf = (intf_thread_t *)init_ctx->p_this;
        vlc_playlist_t *playlist = vlc_intf_GetMainPlaylist(intf);
        mgr->player = vlc_playlist_GetPlayer(playlist);

        
        
        if (mgr->player != NULL) {
            // Activate
            char **ppsz_titles = NULL;
            uint16_t *pi_ids = NULL;
            mgr->pf_control(mgr, EXTENSION_TRIGGER, init_ctx->p_ext, NULL);
            extensions_GetMenu(init_ctx->p_this,
                               mgr,
                               init_ctx->p_ext,&ppsz_titles,&pi_ids);
            mgr->pf_control(mgr, EXTENSION_ACTIVATE, init_ctx->p_ext, NULL);
      
            MUTEX_LOCK(init_ctx->lock);
            break;
        }
        MUTEX_LOCK(init_ctx->lock);
        if (time(NULL) - start_time >= TIMEOUT) {
            msg_Dbg(init_ctx->p_this, "Timeout elapsed, exiting thread.\n");
            break;
        }
    }

    MUTEX_DESTROY(init_ctx->lock);
    free(init_ctx);

    return THREAD_FUNC_RET;
}


int Open_Enabler(vlc_object_t *p_this) {

    // Setup extension manager...
    extensions_manager_t *p_mgr = (extensions_manager_t* ) p_this;
    
    // need the lua extension module to be active before attempting this.
    p_mgr->p_module = module_need( p_mgr, "extension", NULL, false );
    
    if (p_mgr == NULL) {
        msg_Err(p_this, "Could not access extensions manager.");
        return VLC_EGENERIC;
    }

    // Activate the desired extension (TODO: paramertize this)
    const char *extension_name = "xAPI Integration";
    
    extension_t *p_ext = NULL;

    ARRAY_FOREACH(p_ext, p_mgr->extensions) {
        msg_Dbg(p_this ,"checking extension %s...", p_ext->psz_title);
        if (strcmp(p_ext->psz_title, extension_name) == 0) {
            extension_activation_t *ctx = malloc(sizeof(*ctx));
            ctx->p_this = p_this;
            ctx->p_ext = p_ext;
            MUTEX_INIT(ctx->lock);
            THREAD_HANDLE thread;
           
            msg_Dbg(p_this, "yielding extension activation to its own thread.");
            if (THREAD_CREATE(&thread, CheckPlayerInitialization, ctx) != 0) {
                msg_Err(p_this,
                        "Failed to create thread for extension %s...", 
                        p_ext->psz_title);
                free(ctx);
            return VLC_EGENERIC;
            }
            msg_Dbg(p_this, "yielded extension init to its own thread");
            break;
        }
    }

    return VLC_SUCCESS;
}

int Close_Enabler(vlc_object_t *this) {
  // cleanup
}

vlc_module_begin()
  set_shortname( N_("ExtensionEnabler") )
  set_description( N_("VLC Lua Extension Enabler") )
  set_capability( "interface", 0)
  set_callbacks( Open_Enabler, Close_Enabler )
  add_shortcut( "luaext-enabler" )
vlc_module_end()
