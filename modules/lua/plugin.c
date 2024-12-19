#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <sys/stat.h>

#define VLC_MODULE_LICENSE VLC_LICENSE_GPL_2_PLUS

#include "vlc.h"

#include <vlc_configuration.h>
#include <vlc_plugin.h>
#include <vlc_arrays.h>
#include <vlc_charset.h>
#include <vlc_fs.h>
#include <vlc_services_discovery.h>
#include <vlc_stream.h>

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/

#define LUA_TEXT N_("Enable Lua plugins")
#define INTF_TEXT N_("Lua interface")
#define INTF_LONGTEXT N_("Lua interface module to load")

#define CONFIG_TEXT N_("Lua interface configuration")
#define CONFIG_LONGTEXT N_("Lua interface configuration string. Format is: '[\"<interface module name>\"] = { <option> = <value>, ...}, ...'.")
#define PASS_TEXT N_( "Password" )
#define PASS_LONGTEXT N_( "A single password restricts access " \
    "to this interface." )
#define SRC_TEXT N_( "Source directory" )
#define SRC_LONGTEXT NULL
#define INDEX_TEXT N_( "Directory index" )
#define INDEX_LONGTEXT N_( "Allow to build directory index" )

#define TELNETHOST_TEXT N_( "Host" )
#define TELNETHOST_LONGTEXT N_( "This is the host on which the " \
    "interface will listen. It defaults to all network interfaces (0.0.0.0)." \
    " If you want this interface to be available only on the local " \
    "machine, enter \"127.0.0.1\"." )
#define TELNETPORT_TEXT N_( "Port" )
#define TELNETPORT_LONGTEXT N_( "This is the TCP port on which this " \
    "interface will listen. It defaults to 4212." )
#define TELNETPWD_TEXT N_( "Password" )
#define TELNETPWD_LONGTEXT N_( "A single password restricts access " \
    "to this interface." )

/*****************************************************************************
 *
 *****************************************************************************/

vlc_module_begin ()
        set_shortname( N_("Lua") )
        set_description( N_("Lua interpreter") )
        set_subcategory( SUBCAT_INTERFACE_MAIN )

        add_bool( "lua", true, LUA_TEXT, NULL )
        add_string( "lua-intf", "dummy", INTF_TEXT, INTF_LONGTEXT )
        add_string( "lua-config", "", CONFIG_TEXT, CONFIG_LONGTEXT )
        set_capability( "interface", 0 )
        set_callbacks( Open_LuaIntf, Close_LuaIntf )
        add_shortcut( "luaintf" )

    add_submodule ()
        set_section( N_("Lua HTTP"), 0 )
            add_password("http-password", NULL, PASS_TEXT, PASS_LONGTEXT)
            add_string ( "http-src",  NULL, SRC_TEXT,  SRC_LONGTEXT )
            add_bool   ( "http-index", false, INDEX_TEXT, INDEX_LONGTEXT )
        set_capability( "interface", 0 )
        set_callbacks( Open_LuaHTTP, Close_LuaIntf )
        add_shortcut( "luahttp", "http" )
        set_description( N_("Lua HTTP") )

    add_submodule ()
        set_section( N_("Lua Telnet"), 0 )
            add_string( "telnet-host", "localhost", TELNETHOST_TEXT,
                        TELNETHOST_LONGTEXT )
            add_integer( "telnet-port", TELNETPORT_DEFAULT, TELNETPORT_TEXT,
                         TELNETPORT_LONGTEXT )
                change_integer_range( 1, 65535 )
            add_password("telnet-password", NULL, TELNETPWD_TEXT,
                         TELNETPWD_LONGTEXT)
        set_capability( "interface", 0 )
        set_callbacks( Open_LuaTelnet, Close_LuaIntf )
        set_description( N_("Lua Telnet") )
        add_shortcut( "luatelnet", "telnet" )

    add_submodule ()
        set_shortname( N_( "Lua Meta Fetcher" ) )
        set_description( N_("Fetch meta data using lua scripts") )
        set_capability( "meta fetcher", 10 )
        set_callback( FetchMeta )

    add_submodule ()
        set_shortname( N_( "Lua Meta Reader" ) )
        set_description( N_("Read meta data using lua scripts") )
        set_capability( "meta reader", 10 )
        set_callback( ReadMeta )

    add_submodule ()
        add_shortcut( "luaplaylist" )
        set_shortname( N_("Lua Playlist") )
        set_description( N_("Lua Playlist Parser Interface") )
        set_capability( "demux", 5 )
        set_callbacks( Import_LuaPlaylist, Close_LuaPlaylist )

    add_submodule ()
        set_shortname( N_( "Lua Art" ) )
        set_description( N_("Fetch artwork using lua scripts") )
        set_capability( "art finder", 10 )
        set_callback( FindArt )

    add_submodule ()
        set_shortname( N_("Lua Extension") )
        set_description( N_("Lua Extension") )
        add_shortcut( "luaextension" )
        set_capability( "extension", 1 )
        set_callbacks( Open_Extension, Close_Extension )

    add_submodule ()
        set_description( N_("Lua SD Module") )
        add_shortcut( "luasd" )
        set_capability( "services_discovery", 0 )
        add_string( "lua-sd", "", NULL, NULL )
            change_volatile()
        set_callbacks( Open_LuaSD, Close_LuaSD )

    VLC_SD_PROBE_SUBMODULE

vlc_module_end ()
