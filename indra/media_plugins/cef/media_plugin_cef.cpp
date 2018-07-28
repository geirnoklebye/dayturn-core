/**
* @file media_plugin_cef.cpp
* @brief CEF (Chromium Embedding Framework) plugin for LLMedia API plugin system
*
* @cond
* $LicenseInfo:firstyear=2008&license=viewerlgpl$
* Second Life Viewer Source Code
* Copyright (C) 2010, Linden Research, Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
* $/LicenseInfo$
* @endcond
*/

#include "linden_common.h"
#include "indra_constants.h" // for indra keyboard codes

#include "llgl.h"
#include "llsdutil.h"
#include "llplugininstance.h"
#include "llpluginmessage.h"
#include "llpluginmessageclasses.h"
#include "volume_catcher.h"
#include "media_plugin_base.h"

#include <functional>
#include <chrono>

#include "dullahan.h"

////////////////////////////////////////////////////////////////////////////////
//
class MediaPluginCEF :
	public MediaPluginBase
{
public:
	MediaPluginCEF(LLPluginInstance::sendMessageFunction host_send_func, void *host_user_data);
	~MediaPluginCEF();

	/*virtual*/
	void receiveMessage(const char* message_string);

private:
	bool init();

#if LL_USE_DULLAHAN
	void onPageChangedCallback(const unsigned char* pixels, int x, int y,
							   const int width, const int height);
	void onLoadError(int status, const std::string error_text);
	void onOpenPopupCallback(std::string url, std::string target);
#else
	void onPageChangedCallback(unsigned char* pixels,
							   int x, int y, int width, int height,
							   bool is_popup);
	void onNavigateURLCallback(std::string url, std::string target);
#endif
	void onCustomSchemeURLCallback(std::string url);
	void onConsoleMessageCallback(std::string message, std::string source,
								  int line);
	void onStatusMessageCallback(std::string value);
	void onTitleChangeCallback(std::string title);
	void onLoadStartCallback();
	void onLoadEndCallback(int httpStatusCode);
	void onAddressChangeCallback(std::string url);
	bool onHTTPAuthCallback(const std::string host, const std::string realm,
							std::string& username, std::string& password);

	void onCursorChangedCallback(CURSOR_TYPE type);
	void onRequestExitCallback();

	void postDebugMessage(const std::string& msg);

	void authResponse(LLPluginMessage& message);

#if LL_USE_DULLAHAN
	const std::vector<std::string> onFileDialog(dullahan::EFileDialogType dialog_type,
												const std::string dialog_title,
												const std::string default_file,
												const std::string dialog_accept_filter,
												bool& use_default);

	void keyEvent(dullahan::EKeyEvent key_event,
				  LLSD native_key_data = LLSD::emptyMap());
	void unicodeInput(std::string event, LLSD native_key_data = LLSD::emptyMap());
#else
	void onFileDownloadCallback(const std::string filename);
	std::string onFileDialogCallback();

	LLCEFLib::EKeyboardModifier decodeModifiers(std::string& modifiers);
	void keyEvent(LLCEFLib::EKeyEvent key_event, S32 key,
				  LLCEFLib::EKeyboardModifier modifiers,
				  LLSD native_key_data = LLSD::emptyMap());
	void unicodeInput(const std::string& utf8str,
					  LLCEFLib::EKeyboardModifier modifiers,
					  LLSD native_key_data = LLSD::emptyMap());
#endif

	void checkEditState();
	void setVolume();

private:
#if LL_USE_DULLAHAN
	dullahan*					mCEFLib;
#else
	LLCEFLib*					mCEFLib;
#endif

	VolumeCatcher				mVolumeCatcher;
	F32							mCurVolume;

	U32							mMinimumFontSize;

#if LL_LINUX && LL_USE_DULLAHAN
	U32							mLastEmittedKey;
#endif

	std::string 				mHostLanguage;
	std::string					mAuthUsername;
	std::string					mAuthPassword;
	std::string					mPreferredFont;
	std::string					mUserAgent;
	std::string					mPickedFile;
	std::string					mUserDataDir;

	std::vector<std::string>	mPickedFiles;

	bool						mDisableGPU;
	bool						mCookiesEnabled;
	bool						mJavascriptEnabled;
	bool						mPluginsEnabled;
	bool						mAuthOK;
	bool						mRemoteFonts;
	bool						mCanCopy;
	bool						mCanCut;
	bool						mCanPaste;
	bool						mEnableMediaPluginDebugging;
	bool						mCleanupDone;
};

MediaPluginCEF::MediaPluginCEF(LLPluginInstance::sendMessageFunction host_send_func,
							   void* host_user_data)
:	MediaPluginBase(host_send_func, host_user_data),
	mMinimumFontSize(0),
	mHostLanguage("en"),
	mCurVolume(0.5f),		// Set default to a reasonnable level
#if LL_LINUX && LL_USE_DULLAHAN
	mLastEmittedKey(0),
#endif
	mDisableGPU(true),
	mCookiesEnabled(true),
	mJavascriptEnabled(true),
	mPluginsEnabled(true),
	mAuthOK(false),
	mRemoteFonts(true),
	mCanCopy(false),
	mCanCut(false),
	mCanPaste(false),
	mEnableMediaPluginDebugging(true),
	mCleanupDone(false)
{
	mWidth = 0;
	mHeight = 0;
	mDepth = 4;
	mPixels = 0;

#if LL_USE_DULLAHAN
	mCEFLib = new dullahan();
#else
	mCEFLib = new LLCEFLib();
#endif

	setVolume();
}

MediaPluginCEF::~MediaPluginCEF()
{
	std::cerr << "MediaPluginCEF::~MediaPluginCEF called" << std::endl;
	if (!mCleanupDone)
	{
		if (mEnableMediaPluginDebugging)
		{
			std::cerr << "MediaPluginCEF::~MediaPluginCEF calling requestExit()"
					  << std::endl;
		}
		mCEFLib->requestExit();
	}
	// Let some time for CEF to actually cleanup
	ms_sleep(1000);
	mCEFLib->shutdown();
	// Let some time for CEF to actually shut down
	ms_sleep(1000);
}

void MediaPluginCEF::postDebugMessage(const std::string& msg)
{
	if (mEnableMediaPluginDebugging)
	{
		std::stringstream str;
		str << "@Media Msg> " << msg;

		LLPluginMessage debug_message(LLPLUGIN_MESSAGE_CLASS_MEDIA,
									  "debug_message");
		debug_message.setValue("message_text", str.str());
		debug_message.setValue("message_level", "info");
		sendMessage(debug_message);
	}
}

#if LL_USE_DULLAHAN

void MediaPluginCEF::onPageChangedCallback(const unsigned char* pixels, int x,
										   int y, const int width,
										   const int height)
{
	if (mPixels && pixels)
	{
		if (mWidth == width && mHeight == height)
		{
			memcpy(mPixels, pixels, mWidth * mHeight * mDepth);
		}
		else
		{
			mCEFLib->setSize(mWidth, mHeight);
		}
		setDirty(0, 0, mWidth, mHeight);
	}
}

void MediaPluginCEF::onLoadError(int status, const std::string error_text)
{
	std::stringstream msg;
	msg << "<b>Loading error !</b><p>Message: " << error_text;
	msg << "<br />Code: " << status << "</p>";
	mCEFLib->showBrowserMessage(msg.str());
}

#else	// LL_USE_DULLAHAN

void MediaPluginCEF::onPageChangedCallback(unsigned char* pixels,
										   int x, int y, int width, int height,
										   bool is_popup)
{
	if (mPixels && pixels)
	{
		if (is_popup)
		{
			for (int line = 0; line < height; ++line)
			{
				int inverted_y = mHeight - y - height;
				int src = line * width * mDepth;
				int dst = (inverted_y + line) * mWidth * mDepth + x * mDepth;
				if (dst + width * mDepth < mWidth * mHeight * mDepth)
				{
					memcpy(mPixels + dst, pixels + src, width * mDepth);
				}
			}
		}
		else if (mWidth == width && mHeight == height)
		{
			memcpy(mPixels, pixels, mWidth * mHeight * mDepth);
		}

		setDirty(0, 0, mWidth, mHeight);
	}
}

#endif // LL_USE_DULLAHAN

void MediaPluginCEF::onConsoleMessageCallback(std::string message,
											  std::string source, int line)
{
	std::stringstream str;
	str << "Console message: " << message << " in file(" << source
		<< ") at line " << line;
	postDebugMessage(str.str());
}

void MediaPluginCEF::onStatusMessageCallback(std::string value)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"status_text");
	message.setValue("status", value);
	sendMessage(message);
}

void MediaPluginCEF::onTitleChangeCallback(std::string title)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "name_text");
	message.setValue("name", title);
	message.setValue("artist", "");
#if LL_USE_DULLAHAN
	message.setValueBoolean("history_back_available", mCEFLib->canGoBack());
	message.setValueBoolean("history_forward_available",
							mCEFLib->canGoForward());
#endif
	sendMessage(message);
}

void MediaPluginCEF::onLoadStartCallback()
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"navigate_begin");
#if 0	// not easily available here in CEF - needed ?
	message.setValue("uri", event.getEventUri());
#endif
	message.setValueBoolean("history_back_available", mCEFLib->canGoBack());
	message.setValueBoolean("history_forward_available",
							mCEFLib->canGoForward());
	sendMessage(message);
}

void MediaPluginCEF::onLoadEndCallback(int httpStatusCode)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"navigate_complete");
#if 0	// not easily available here in CEF - needed ?
	message.setValue("uri", event.getEventUri());
#endif
	message.setValueS32("result_code", httpStatusCode);
	message.setValueBoolean("history_back_available", mCEFLib->canGoBack());
	message.setValueBoolean("history_forward_available",
							mCEFLib->canGoForward());
	sendMessage(message);
}

void MediaPluginCEF::onAddressChangeCallback(std::string url)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"location_changed");
	message.setValue("uri", url);
	sendMessage(message);
}

#if LL_USE_DULLAHAN
void MediaPluginCEF::onOpenPopupCallback(std::string url, std::string target)
#else
void MediaPluginCEF::onNavigateURLCallback(std::string url, std::string target)
#endif
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"click_href");
	message.setValue("uri", url);
	message.setValue("target", target);
	message.setValue("uuid", "");	// not used right now
	sendMessage(message);
}

void MediaPluginCEF::onCustomSchemeURLCallback(std::string url)
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER,
							"click_nofollow");
	message.setValue("uri", url);
	// *TODO: differentiate between click and navigate to
	message.setValue("nav_type", "clicked");
	sendMessage(message);
}

bool MediaPluginCEF::onHTTPAuthCallback(const std::string host,
										const std::string realm,
										std::string& username,
										std::string& password)
{
	mAuthOK = false;

	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "auth_request");
	message.setValue("url", host);
	message.setValue("realm", realm);
	message.setValueBoolean("blocking_request", true);

	// The "blocking_request" key in the message means this sendMessage call
	// will block until a response is received.
	sendMessage(message);

	if (mAuthOK)
	{
		username = mAuthUsername;
		password = mAuthPassword;
	}

	return mAuthOK;
}

#if LL_USE_DULLAHAN

const std::vector<std::string> MediaPluginCEF::onFileDialog(dullahan::EFileDialogType dialog_type,
															const std::string dialog_title,
															const std::string default_file,
															const std::string dialog_accept_filter,
															bool& use_default)
{
	// Never use the default CEF file picker
	use_default = false;

	if (dialog_type == dullahan::FD_OPEN_FILE ||
		dialog_type == dullahan::FD_OPEN_MULTIPLE_FILES)
	{
		mPickedFiles.clear();

		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "pick_file");
		message.setValueBoolean("blocking_request", true);
		message.setValueBoolean("multiple_files",
								dialog_type == dullahan::FD_OPEN_MULTIPLE_FILES);

		// The "blocking_request" key in the message means this sendMessage
		// call will block until a response is received.
		sendMessage(message);

		return mPickedFiles;
	}
	else if (dialog_type == dullahan::FD_SAVE_FILE)
	{
		mAuthOK = false;

		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "file_download");
		message.setValue("filename", default_file);

		sendMessage(message);
	}

	return std::vector<std::string>();
}

#else	// LL_USE_DULLAHAN

void MediaPluginCEF::onFileDownloadCallback(const std::string filename)
{
	mAuthOK = false;

	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "file_download");
	message.setValue("filename", filename);

	sendMessage(message);
}

std::string MediaPluginCEF::onFileDialogCallback()
{
	mPickedFile.clear();

	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "pick_file");
	message.setValueBoolean("blocking_request", true);

	// The "blocking_request" key in the message means this sendMessage call
	// will block until a response is received.
	sendMessage(message);

	return mPickedFile;
}

#endif	// LL_USE_DULLAHAN

void MediaPluginCEF::onCursorChangedCallback(CURSOR_TYPE type)
{
	std::string name = "";

	switch (type)
	{
		case CURSOR_TYPE_POINTER:
			name = "arrow";
			break;
		case CURSOR_TYPE_BEAM:
			name = "ibeam";
			break;
		case CURSOR_TYPE_NS_RESIZE:
			name = "splitv";
			break;
		case CURSOR_TYPE_EW_RESIZE:
			name = "splith";
			break;
		case CURSOR_TYPE_HAND:
			name = "hand";
			break;

		default:
			LL_WARNS()<< "Unknown cursor ID: " << (S32)type << LL_ENDL;
			break;
	}

	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "cursor_changed");
	message.setValue("name", name);
	sendMessage(message);
}

void MediaPluginCEF::onRequestExitCallback()
{
	if (mEnableMediaPluginDebugging)
	{
		std::cerr << "MediaPluginCEF::onRequestExitCallback called"
				  << std::endl;
	}
	mCleanupDone = true;

	LLPluginMessage message("base", "goodbye");
	sendMessage(message);

#if 0
	mCEFLib->shutdown();
#else
	mDeleteMe = true;
#endif
}

void MediaPluginCEF::authResponse(LLPluginMessage& message)
{
	mAuthOK = message.getValueBoolean("ok");
	if (mAuthOK)
	{
		mAuthUsername = message.getValue("username");
		mAuthPassword = message.getValue("password");
	}
}

void MediaPluginCEF::receiveMessage(const char* message_string)
{
	if (mEnableMediaPluginDebugging)
	{
		std::string msg(message_string);
		// Do not spam cerr with a gazillon of idle messages...
		if (msg.find("<string>idle</string>") == std::string::npos)
		{
			std::cerr << "MediaPluginCEF::receiveMessage: received message: \""
					  << message_string << "\"" << std::endl;
		}
	}

	LLPluginMessage message_in;

	if (message_in.parse(message_string) >= 0)
	{
		std::string message_class = message_in.getClass();
		std::string message_name = message_in.getName();
		if (message_class == LLPLUGIN_MESSAGE_CLASS_BASE)
		{
			if (message_name == "init")
			{
				LLPluginMessage message("base", "init_response");
				LLSD versions = LLSD::emptyMap();
				versions[LLPLUGIN_MESSAGE_CLASS_BASE] = LLPLUGIN_MESSAGE_CLASS_BASE_VERSION;
				versions[LLPLUGIN_MESSAGE_CLASS_MEDIA] = LLPLUGIN_MESSAGE_CLASS_MEDIA_VERSION;
				versions[LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER] = LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER_VERSION;
				message.setValueLLSD("versions", versions);

				std::string plugin_version = "CEF3 plugin v";
#if LL_USE_DULLAHAN
				plugin_version.append(CEF_VERSION);
#else
				plugin_version.append(LLCEFLIB_VERSION);
#endif
				message.setValue("plugin_version", plugin_version);
				sendMessage(message);
			}
			else if (message_name == "idle")
			{
				mCEFLib->update();
				mVolumeCatcher.pump();
				checkEditState();
			}
			else if (message_name == "cleanup")
			{
				if (!mCleanupDone)
				{
					mCEFLib->requestExit();
				}
			}
			else if (message_name == "shm_added")
			{
				SharedSegmentInfo info;
				info.mAddress = message_in.getValuePointer("address");
				info.mSize = (size_t)message_in.getValueS32("size");
				std::string name = message_in.getValue("name");

				mSharedSegments.insert(SharedSegmentMap::value_type(name, info));

			}
			else if (message_name == "shm_remove")
			{
				std::string name = message_in.getValue("name");

				SharedSegmentMap::iterator iter = mSharedSegments.find(name);
				if (iter != mSharedSegments.end())
				{
					if (mPixels == iter->second.mAddress)
					{
						mPixels = NULL;
						mTextureSegmentName.clear();
					}
					mSharedSegments.erase(iter);
				}
				else if (mEnableMediaPluginDebugging)
				{
					std::cerr << "MediaPluginCEF::receiveMessage: unknown shared memory region !"
							  << std::endl;
				}

				LLPluginMessage message("base", "shm_remove_response");
				message.setValue("name", name);
				sendMessage(message);
			}
			else if (mEnableMediaPluginDebugging)
			{
				std::cerr << "MediaPluginCEF::receiveMessage: unknown base message: "
						  << message_name << std::endl;
			}
		}
		else if (message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA_TIME)
		{
			if (mEnableMediaPluginDebugging)
			{
				std::cerr << "MediaPluginCEF::receiveMessage: class: "
						  << message_class << " - name: " << message_name
						  << std::endl;
			}
		}
		else if (message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA)
		{
			if (message_name == "init")
			{
#if LL_USE_DULLAHAN
				mCEFLib->setOnPageChangedCallback(BINDFUNC(&MediaPluginCEF::onPageChangedCallback, this, PH1, PH2, PH3, PH4, PH5));
				mCEFLib->setOnOpenPopupCallback(BINDFUNC(&MediaPluginCEF::onOpenPopupCallback, this, PH1, PH2));
				mCEFLib->setOnFileDialogCallback(BINDFUNC(&MediaPluginCEF::onFileDialog, this, PH1, PH2, PH3, PH4, PH5));
				mCEFLib->setOnLoadErrorCallback(BINDFUNC(&MediaPluginCEF::onLoadError, this, PH1, PH2));
#else
				mCEFLib->setOnPageChangedCallback(BINDFUNC(&MediaPluginCEF::onPageChangedCallback, this, PH1, PH2, PH3, PH4, PH5, PH6));
				mCEFLib->setOnNavigateURLCallback(BINDFUNC(&MediaPluginCEF::onNavigateURLCallback, this, PH1, PH2));
				mCEFLib->setOnFileDownloadCallback(BINDFUNC(&MediaPluginCEF::onFileDownloadCallback, this, PH1));
				mCEFLib->setOnFileDialogCallback(BINDFUNC(&MediaPluginCEF::onFileDialogCallback, this));
#endif
				mCEFLib->setOnCustomSchemeURLCallback(BINDFUNC(&MediaPluginCEF::onCustomSchemeURLCallback, this, PH1));
				mCEFLib->setOnConsoleMessageCallback(BINDFUNC(&MediaPluginCEF::onConsoleMessageCallback, this, PH1, PH2, PH3));
				mCEFLib->setOnStatusMessageCallback(BINDFUNC(&MediaPluginCEF::onStatusMessageCallback, this, PH1));
				mCEFLib->setOnTitleChangeCallback(BINDFUNC(&MediaPluginCEF::onTitleChangeCallback, this, PH1));
				mCEFLib->setOnLoadStartCallback(BINDFUNC(&MediaPluginCEF::onLoadStartCallback, this));
				mCEFLib->setOnLoadEndCallback(BINDFUNC(&MediaPluginCEF::onLoadEndCallback, this, PH1));
				mCEFLib->setOnAddressChangeCallback(BINDFUNC(&MediaPluginCEF::onAddressChangeCallback, this, PH1));
				mCEFLib->setOnHTTPAuthCallback(BINDFUNC(&MediaPluginCEF::onHTTPAuthCallback, this, PH1, PH2, PH3, PH4));
				mCEFLib->setOnCursorChangedCallback(BINDFUNC(&MediaPluginCEF::onCursorChangedCallback, this, PH1));
				mCEFLib->setOnRequestExitCallback(BINDFUNC(&MediaPluginCEF::onRequestExitCallback, this));

				BROWSER_SETTING settings;
				settings.initial_width = 1024;
				settings.initial_height = 1024;
				settings.user_agent_substring =
					mCEFLib->makeCompatibleUserAgentString(mUserAgent);
				settings.cookies_enabled = mCookiesEnabled;
				settings.cache_enabled = true;
				settings.accept_language_list = mHostLanguage;
				settings.javascript_enabled = mJavascriptEnabled;
				settings.plugins_enabled = mPluginsEnabled;
				// MAINT-6060 - WebRTC media removed until we can add
				// granualrity/query UI
				settings.media_stream_enabled = false;
#if LL_USE_DULLAHAN
				settings.background_color = 0xffffffff;
				settings.disable_gpu = mDisableGPU;
				settings.flash_enabled = mPluginsEnabled;
				settings.flip_mouse_y = false;
				settings.flip_pixels_y = true;
				settings.frame_rate = 60;
				settings.force_wave_audio = true;
				settings.java_enabled = false;
				settings.webgl_enabled = true;
				settings.remote_debugging_port = 0;
				std::vector<std::string> custom_schemes;
				custom_schemes.push_back("secondlife");
				custom_schemes.push_back("x-grid-location-info");
				mCEFLib->setCustomSchemes(custom_schemes);
#endif
#if HB_CEF_EXTENDED
				// Not implemented in LL's pre-compiled llceflib or Dullahan
				// versions
				settings.minimum_font_size = mMinimumFontSize;
				settings.remote_fonts = mRemoteFonts;
				settings.preferred_font = mPreferredFont;
				settings.user_data_dir = mUserDataDir;
				settings.debug = mEnableMediaPluginDebugging;
#else
				settings.cookie_store_path = mUserDataDir + "cef_cookies";
				settings.cache_path = mUserDataDir + "cef_cache";
# if LL_USE_DULLAHAN
				settings.log_file = mUserDataDir + "cef_log.txt";
				settings.log_verbose = mEnableMediaPluginDebugging;
# endif
#endif

				bool result = mCEFLib->init(settings);
#if 0			// *TODO - return something to indicate failure
				if (!result)
				{
					MessageBoxA(0, "FAIL INIT", 0, 0);
				}
#endif
				if (!result && mEnableMediaPluginDebugging)
				{
					std::cerr << "MediaPluginCEF::receiveMessage: mCEFLib->init() failed"
							  << std::endl;
				}

				// Plugin gets to decide the texture parameters to use.
				mDepth = 4;
				LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA,
										"texture_params");
				message.setValueS32("default_width", 1024);
				message.setValueS32("default_height", 1024);
				message.setValueS32("depth", mDepth);
				message.setValueU32("internalformat", GL_RGB);
				message.setValueU32("format", GL_BGRA);
				message.setValueU32("type", GL_UNSIGNED_BYTE);
				message.setValueBoolean("coords_opengl", true);
				sendMessage(message);
			}
			else if (message_name == "set_user_data_path")
			{
				// Note: path always got a trailing platform-specific directory
				// delimiter
				mUserDataDir = message_in.getValue("path");
			}
			else if (message_name == "size_change")
			{
				std::string name = message_in.getValue("name");
				S32 width = message_in.getValueS32("width");
				S32 height = message_in.getValueS32("height");
				S32 texture_width = message_in.getValueS32("texture_width");
				S32 texture_height = message_in.getValueS32("texture_height");

				if (!name.empty())
				{
					// Find the shared memory region with this name
					SharedSegmentMap::iterator iter = mSharedSegments.find(name);
					if (iter != mSharedSegments.end())
					{
						mPixels = (unsigned char*)iter->second.mAddress;
						mWidth = width;
						mHeight = height;

						mTextureWidth = texture_width;
						mTextureHeight = texture_height;
#if LL_USE_DULLAHAN
						mCEFLib->setSize(mWidth, mHeight);
#endif
					}
				}

				mCEFLib->setSize(mWidth, mHeight);

				LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA,
										"size_change_response");
				message.setValue("name", name);
				message.setValueS32("width", width);
				message.setValueS32("height", height);
				message.setValueS32("texture_width", texture_width);
				message.setValueS32("texture_height", texture_height);
				sendMessage(message);

			}
			else if (message_name == "set_language_code")
			{
				mHostLanguage = message_in.getValue("language");
			}
			else if (message_name == "load_uri")
			{
				std::string uri = message_in.getValue("uri");
				mCEFLib->navigate(uri);
			}
			else if (message_name == "set_cookie")
			{
				std::string uri = message_in.getValue("uri");
				std::string name = message_in.getValue("name");
				std::string value = message_in.getValue("value");
				std::string domain = message_in.getValue("domain");
				std::string path = message_in.getValue("path");
				bool httponly = message_in.getValueBoolean("httponly");
				bool secure = message_in.getValueBoolean("secure");
				mCEFLib->setCookie(uri, name, value, domain, path, httponly,
								   secure);
			}
			else if (message_name == "mouse_event")
			{
				std::string event = message_in.getValue("event");

				S32 x = message_in.getValueS32("x");
				S32 y = message_in.getValueS32("y");

				BROWSER_MOUSE_BUTTON btn = BROWSER_MB_LEFT;
				S32 button = message_in.getValueS32("button");
#if 1			// Do not transmit middle or right clicks
				if (button == 1 || button == 2) return;
#else
				if (button == 1) btn = BROWSER_MB_RIGHT;
				if (button == 2) btn = BROWSER_MB_MIDDLE;
#endif
#if 0			// Not used for now
				std::string modifiers = message_in.getValue("modifiers");
#endif
				if (event == "down")
				{
					mCEFLib->mouseButton(btn, BROWSER_ME_DOWN, x, y);
#if LL_USE_DULLAHAN
					mCEFLib->setFocus();
#else
					mCEFLib->setFocus(true);
#endif
					std::stringstream str;
					str << "Mouse down at = " << x << ", " << y;
					postDebugMessage(str.str());
				}
				else if (event == "up")
				{
					mCEFLib->mouseButton(btn, BROWSER_ME_UP, x, y);
					std::stringstream str;
					str << "Mouse up at = " << x << ", " << y;
					postDebugMessage(str.str());
				}
				else if (event == "double_click")
				{
					mCEFLib->mouseButton(btn, BROWSER_ME_DBLE_CLICK, x, y);
				}
				else
				{
					mCEFLib->mouseMove(x, y);
				}
			}
			else if (message_name == "scroll_event")
			{
				S32 x = 40 * message_in.getValueS32("x");
				S32 y = -40 * message_in.getValueS32("y");
				mCEFLib->mouseWheel(x, y);
			}
			else if (message_name == "text_event")
			{
				LLSD native_key_data = message_in.getValueLLSD("native_key_data");
#if LL_USE_DULLAHAN
				std::string event = message_in.getValue("event");
				unicodeInput(event, native_key_data);
#else
				std::string text = message_in.getValue("text");
				std::string modifiers = message_in.getValue("modifiers");
				unicodeInput(text, decodeModifiers(modifiers),
							 native_key_data);
#endif
			}
			else if (message_name == "key_event")
			{
				LLSD native_key_data = message_in.getValueLLSD("native_key_data");
				std::string event = message_in.getValue("event");
				// Treat unknown events as key-up for safety.
				BROWSER_KEY_EVENT key_event = BROWSER_KE_UP;
				if (event == "down")
				{
					key_event = BROWSER_KE_DOWN;
				}
				else if (event == "repeat")
				{
					key_event = BROWSER_KE_REPEAT;
				}
#if LL_USE_DULLAHAN
				keyEvent(key_event, native_key_data);
#else
				S32 key = message_in.getValueS32("key");
				std::string modifiers = message_in.getValue("modifiers");
				keyEvent(key_event, key, decodeModifiers(modifiers),
						 native_key_data);
#endif
			}
			else if (message_name == "enable_media_plugin_debugging")
			{
				mEnableMediaPluginDebugging = message_in.getValueBoolean("enable");
			}
			else if (message_name == "pick_file_response")
			{
				mPickedFile = message_in.getValue("file");
#if LL_USE_DULLAHAN
				LLSD file_list = message_in.getValueLLSD("file_list");
				for (LLSD::array_const_iterator iter = file_list.beginArray(),
												end = file_list.endArray();
					 iter != end; ++iter)
				{
					mPickedFiles.push_back((*iter).asString());
				}
				if (mPickedFiles.empty() && !mPickedFile.empty())
				{
					mPickedFiles.push_back(mPickedFile);
				}
#endif
			}
			else if (message_name == "auth_response")
			{
				authResponse(message_in);
			}
			else if (message_name == "edit_copy")
			{
				mCEFLib->editCopy();
			}
			else if (message_name == "edit_cut")
			{
				mCEFLib->editCut();
			}
			else if (message_name == "edit_paste")
			{
				mCEFLib->editPaste();
			}
		}
		else if (message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA_BROWSER)
		{
			if (message_name == "set_page_zoom_factor")
			{
				F32 factor = (F32)message_in.getValueReal("factor");
				mCEFLib->setPageZoom(factor);
			}
			else if (message_name == "cookies_enabled")
			{
				mCookiesEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "show_web_inspector")
			{
#if LL_USE_DULLAHAN
				mCEFLib->showDevTools();
#else
				mCEFLib->showDevTools(true);
#endif
			}
			else if (message_name == "plugins_enabled")
			{
				mPluginsEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "javascript_enabled")
			{
				mJavascriptEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "gpu_disabled")
			{
				mDisableGPU = message_in.getValueBoolean("disable");
			}
			else if (message_name == "minimum_font_size")
			{
				mMinimumFontSize = message_in.getValueU32("size");
			}
			else if (message_name == "remote_fonts")
			{
				mRemoteFonts = message_in.getValueBoolean("enable");
			}
			else if (message_name == "preferred_font")
			{
				mPreferredFont = message_in.getValue("font_family");
			}
			else if (message_name == "browse_stop")
			{
				mCEFLib->stop();
			}
			else if (message_name == "browse_reload")
			{
				bool ignore_cache = true;
				mCEFLib->reload(ignore_cache);
			}
			else if (message_name == "browse_forward")
			{
				mCEFLib->goForward();
			}
			else if (message_name == "browse_back")
			{
				mCEFLib->goBack();
			}
			else if (message_name == "cookies_enabled")
			{
				mCookiesEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "clear_cookies")
			{
				mCEFLib->deleteAllCookies();
			}
			else if (message_name == "set_user_agent")
			{
				mUserAgentSubtring = message_in.getValue("user_agent");
			}
			else if (message_name == "show_web_inspector")
			{
				mCEFLib->showDevTools();
			}
			else if (message_name == "plugins_enabled")
			{
				mPluginsEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "javascript_enabled")
			{
				mJavascriptEnabled = message_in.getValueBoolean("enable");
			}
			else if (message_name == "gpu_disabled")
			{
				mDisableGPU = message_in.getValueBoolean("disable");
			}
		}
        else if (message_class == LLPLUGIN_MESSAGE_CLASS_MEDIA_TIME)
        {
            if (message_name == "set_volume")
            {
				F32 volume = (F32)message_in.getValueReal("volume");
				mCurVolume = volume;
                setVolume();
            }
        }
        else
		{
		};
	}
}

////////////////////////////////////////////////////////////////////////////////
//
void MediaPluginCEF::keyEvent(dullahan::EKeyEvent key_event, LLSD native_key_data = LLSD::emptyMap())
{

#if LL_LINUX
		native_scan_code = (uint32_t)(native_key_data["sdl_sym"].asInteger());
		native_virtual_key = (uint32_t)(native_key_data["virtual_key"].asInteger());
		native_modifiers = (uint32_t)(native_key_data["cef_modifiers"].asInteger());

		if( native_scan_code == '\n' )
			native_scan_code = '\r';
#endif
#if LL_DARWIN
	U32 event_modifiers = native_key_data["event_modifiers"].asInteger();
	U32 event_keycode = native_key_data["event_keycode"].asInteger();
	U32 event_chars = native_key_data["event_chars"].asInteger();
	U32 event_umodchars = native_key_data["event_umodchars"].asInteger();
	bool event_isrepeat = native_key_data["event_isrepeat"].asBoolean();

	// adding new code below in unicodeInput means we don't send ascii chars
	// here too or we get double key presses on a mac.  
	bool esc_key = (event_umodchars == 27);
	if (esc_key || ((unsigned char)event_chars < 0x10 || (unsigned char)event_chars >= 0x7f ))
	{
		mCEFLib->nativeKeyboardEventOSX(key_event, event_modifiers, 
										event_keycode, event_chars, 
										event_umodchars, event_isrepeat);
	}
#elif LL_WINDOWS
	U32 msg = ll_U32_from_sd(native_key_data["msg"]);
	U32 wparam = ll_U32_from_sd(native_key_data["w_param"]);
	U64 lparam = ll_U32_from_sd(native_key_data["l_param"]);
	mCEFLib->nativeKeyboardEventWin(msg, wparam, lparam);
#elif LL_LINUX
	U32 native_scan_code = native_key_data["sdl_key_sym"].asInteger();
	if (native_scan_code == (U32)'\n')
	{
		native_scan_code = (U32)'\r';
	}
	// HACK: avoid double key events because of unicodeInput() being called
	// as well for some (many but not all) keys...
	if (key_event == dullahan::KE_KEY_DOWN)
	{
		mLastEmittedKey = native_scan_code;
	}
	else
	{
		mLastEmittedKey = 0;
	}
	U32 native_virtual_key = native_key_data["virtual_key"].asInteger();
	U32 native_modifiers = native_key_data["cef_modifiers"].asInteger();
	if (mEnableMediaPluginDebugging)
	{
		std::cerr << "MediaPluginCEF::keyEvent: key_event = " << key_event
				  << " - native_scan_code =  " << native_scan_code
				  << " - native_virtual_key =  " << native_virtual_key
				  << " - native_modifiers =  " << native_modifiers
				  << std::endl;
	}
	mCEFLib->nativeKeyboardEventLin(key_event, native_scan_code,
									native_virtual_key, native_modifiers);
#endif
}

void MediaPluginCEF::unicodeInput(std::string event, LLSD native_key_data)
{
#if LL_DARWIN
	// I did not think this code was needed for MacOS but without it, the IME
	// input in japanese (and likely others too) does not work correctly.
	// see maint-7654
	U32 event_modifiers = native_key_data["event_modifiers"].asInteger();
	U32 event_keycode = native_key_data["event_keycode"].asInteger();
	U32 event_chars = native_key_data["event_chars"].asInteger();
	U32 event_umodchars = native_key_data["event_umodchars"].asInteger();
	bool is_repeat = native_key_data["event_isrepeat"].asBoolean();
    dullahan::EKeyEvent key_event = event == "down" ? dullahan::KE_KEY_DOWN
													: dullahan::KE_KEY_UP;
	mCEFLib->nativeKeyboardEventOSX(key_event, event_modifiers, 
									event_keycode, event_chars, 
									event_umodchars, is_repeat);
#elif LL_WINDOWS
	event = ""; // not needed here but prevents unused var warning as error
	U32 msg = ll_U32_from_sd(native_key_data["msg"]);
	U32 wparam = ll_U32_from_sd(native_key_data["w_param"]);
	U64 lparam = ll_U32_from_sd(native_key_data["l_param"]);
	mCEFLib->nativeKeyboardEventWin(msg, wparam, lparam);
#elif LL_LINUX
	U32 native_scan_code = native_key_data["sdl_key_sym"].asInteger();
	if (native_scan_code == (U32)'\n')
	{
		native_scan_code = (U32)'\r';
	}
	if (mLastEmittedKey && mLastEmittedKey == native_scan_code)
	{
		mLastEmittedKey = 0;
		return;
	}
	mLastEmittedKey = 0;
	U32 native_virtual_key = native_key_data["virtual_key"].asInteger();
	U32 native_modifiers = native_key_data["cef_modifiers"].asInteger();
	if (mEnableMediaPluginDebugging)
	{
		std::cerr << "MediaPluginCEF::keyEvent: native_scan_code =  "
				  << native_scan_code << " - native_virtual_key =  "
				  << native_virtual_key << " - native_modifiers =  "
				  << native_modifiers << std::endl;
	}
	mCEFLib->nativeKeyboardEventLin(dullahan::KE_KEY_DOWN, native_scan_code,
									native_virtual_key, native_modifiers);
#endif
}

#else // LL_USE_DULLAHAN

LLCEFLib::EKeyboardModifier MediaPluginCEF::decodeModifiers(std::string& modifiers)
{
	int result = 0;

	if (modifiers.find("shift") != std::string::npos)
	{
		result |= LLCEFLib::KM_MODIFIER_SHIFT;
	}

	if (modifiers.find("alt") != std::string::npos)
	{
		result |= LLCEFLib::KM_MODIFIER_ALT;
	}

	if (modifiers.find("control") != std::string::npos)
	{
		result |= LLCEFLib::KM_MODIFIER_CONTROL;
	}

	if (modifiers.find("meta") != std::string::npos)
	{
		result |= LLCEFLib::KM_MODIFIER_META;
	}

	return (LLCEFLib::EKeyboardModifier)result;
}

void MediaPluginCEF::keyEvent(LLCEFLib::EKeyEvent key_event, S32 key,
							  LLCEFLib::EKeyboardModifier modifiers,
							  LLSD native_key_data)
{
	if (key == KEY_SHIFT || key == KEY_CONTROL || key == KEY_ALT)
	{
		// Ignore these (avoids having selected text spuriously unselected)
		return;
	}

# if LL_DARWIN
	if (!native_key_data.has("event_type") ||
		!native_key_data.has("event_modifiers") ||
		!native_key_data.has("event_keycode") ||
		!native_key_data.has("event_isrepeat"))
	{
		return;
	}

	U32 native_event_type = native_key_data["event_type"].asInteger();
	if (!native_event_type) return;

	U32 native_modifiers = native_key_data["event_modifiers"].asInteger();
	U32 native_keycode = native_key_data["event_keycode"].asInteger();

	char native_chars = 0;
	if (!native_key_data["event_chars"].isUndefined())
	{
		native_chars = (char)native_key_data["event_chars"].asInteger();
	}

	char native_uchars = 0;
	if (!native_key_data["event_umodchars"].isUndefined())
	{
		native_uchars = (char)native_key_data["event_umodchars"].asInteger();
	}

	bool event_is_repeat = native_key_data["event_isrepeat"].asBoolean();

	mCEFLib->keyboardEventOSX(native_event_type, native_modifiers,
							  native_chars ? &native_chars : NULL,
							  native_uchars ? &native_uchars : NULL,
							  event_is_repeat, native_keycode);
# elif LL_WINDOWS
	U32 msg = ll_U32_from_sd(native_key_data["msg"]);
	U32 wparam = ll_U32_from_sd(native_key_data["w_param"]);
	U64 lparam = ll_U32_from_sd(native_key_data["l_param"]);
	mCEFLib->nativeKeyboardEvent(msg, wparam, lparam);
# elif LL_LINUX
	// The incoming values for 'key' will be the ones from indra_constants.h
	std::string utf8_text;

	if (key < 128)
	{
		// Low-ascii characters need to get passed through.
		utf8_text = (char)key;
	}

	// Any special-case handling we want to do for particular keys...
	switch ((KEY)key)
	{
		// ASCII codes for some standard keys
		case KEY_BACKSPACE:		utf8_text = (char)8;	break;
		case KEY_TAB:			utf8_text = (char)9;	break;
		case KEY_RETURN:		utf8_text = (char)13;	break;
		case KEY_PAD_RETURN:	utf8_text = (char)13;	break;
		case KEY_ESCAPE:		utf8_text = (char)27;	break;

		default:
			break;
	}

	U32 native_scan_code = native_key_data["scan_code"].asInteger();
	if (native_scan_code == (U32)'\n')
	{
		native_scan_code = (U32)'\r';
	}
	U32 native_virtual_key = native_key_data["virtual_key"].asInteger();
	U32 native_modifiers = native_key_data["cef_modifiers"].asInteger();
	if (mEnableMediaPluginDebugging)
	{
		std::cerr << "MediaPluginCEF::keyEvent: keyevent="
				  << (S32)key_event
				  << " - key=" << std::hex << key
				  << " - character=" << utf8_text.c_str()
				  << " - modifier=" << std::hex << (S32)modifiers
				  << " - native_scan_code=" << native_scan_code
				  << " - native_virtual_key=" << native_virtual_key
				  << " - native_modifiers=" << native_modifiers
				  << std::dec << std::endl;
	}
	mCEFLib->keyboardEvent(key_event, (U32)key, utf8_text.c_str(),
						   modifiers, native_scan_code, native_virtual_key,
						   native_modifiers);
# endif
}

void MediaPluginCEF::unicodeInput(const std::string& utf8str,
								  LLCEFLib::EKeyboardModifier modifiers,
								  LLSD native_key_data)
{
# if LL_DARWIN
	if (!native_key_data.has("event_chars") ||
		!native_key_data.has("event_umodchars") ||
		!native_key_data.has("event_keycode") ||
		!native_key_data.has("event_modifiers"))
	{
		return;
	}

	U32 unicode_char = native_key_data["event_chars"].asInteger();
	U32 unmodified_char = native_key_data["event_umodchars"].asInteger();
	U32 key_code = native_key_data["event_keycode"].asInteger();
	U32 raw_modifiers = native_key_data["event_modifiers"].asInteger();

	mCEFLib->injectUnicodeText(unicode_char, unmodified_char, key_code,
							   raw_modifiers);
# elif LL_WINDOWS
	U32 msg = ll_U32_from_sd(native_key_data["msg"]);
	U32 wparam = ll_U32_from_sd(native_key_data["w_param"]);
	U64 lparam = ll_U32_from_sd(native_key_data["l_param"]);
	mCEFLib->nativeKeyboardEvent(msg, wparam, lparam);
# elif LL_LINUX
	U32 key = KEY_NONE;

	if (utf8str.size() == 1)
	{
		// The only way a utf8 string can be one byte long is if it's actually
		// a single 7-bit ascii character.
		// In this case, use it as the key value.
		key = utf8str[0];
	}

	U32 native_scan_code = native_key_data["scan_code"].asInteger();
	if (native_scan_code == (U32)'\n')
	{
		native_scan_code = (U32)'\r';
	}
	U32 native_virtual_key = native_key_data["virtual_key"].asInteger();
	U32 native_modifiers = native_key_data["cef_modifiers"].asInteger();
	if (mEnableMediaPluginDebugging)
	{
		std::cerr << "MediaPluginCEF::unicodeInput: key=" << std::hex << key
				  << " - character=" << utf8str.c_str()
				  << " - modifier=" << std::hex << modifiers
				  << " - native_virtual_key=" << native_virtual_key
				  << " - native_modifiers=" << native_modifiers
				  << std::dec << std::endl;
	}

	mCEFLib->keyboardEvent(LLCEFLib::KE_KEY_DOWN, (U32)key,
						   utf8str.c_str(), modifiers, 0, native_virtual_key,
						   native_modifiers);
	mCEFLib->keyboardEvent(LLCEFLib::KE_KEY_UP, (U32)key,
						   utf8str.c_str(), modifiers, 0, native_virtual_key,
						   native_modifiers);
# endif
}

#endif // LL_USE_DULLAHAN

void MediaPluginCEF::checkEditState()
{
	bool can_copy = mCEFLib->editCanCopy();
	bool can_cut = mCEFLib->editCanCut();
	bool can_paste = mCEFLib->editCanPaste();

	if (can_copy != mCanCopy || can_cut != mCanCut || can_paste != mCanPaste)
	{
		LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "edit_state");

		if (can_copy != mCanCopy)
		{
			mCanCopy = can_copy;
			message.setValueBoolean("copy", can_copy);
		}
		if (can_cut != mCanCut)
		{
			mCanCut = can_cut;
			message.setValueBoolean("cut", can_cut);
		}
		if (can_paste != mCanPaste)
		{
			mCanPaste = can_paste;
			message.setValueBoolean("paste", can_paste);
		}
	}
}

void MediaPluginCEF::setVolume()
{
	mVolumeCatcher.setVolume(mCurVolume);
}

bool MediaPluginCEF::init()
{
	LLPluginMessage message(LLPLUGIN_MESSAGE_CLASS_MEDIA, "name_text");
	message.setValue("name", "CEF Plugin");
	sendMessage(message);

	return true;
}

int init_media_plugin(LLPluginInstance::sendMessageFunction host_send_func,
					  void* host_user_data,
					  LLPluginInstance::sendMessageFunction* plugin_send_func,
					  void** plugin_user_data)
{
	MediaPluginCEF* self = new MediaPluginCEF(host_send_func, host_user_data);
	*plugin_send_func = MediaPluginCEF::staticReceiveMessage;
	*plugin_user_data = (void*)self;

	return 0;
}
