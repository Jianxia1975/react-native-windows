// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>

#include <DevSettings.h>
#include <Folly/dynamic.h>
#include "XamlView.h"

#include <functional>
#include <string>

namespace facebook {
namespace react {
struct INativeUIManager;
}
} // namespace facebook

namespace react {
namespace uwp {

struct IXamlRootView;
class ExpressionAnimationStore;

typedef unsigned int LiveReloadCallbackCookie;
typedef unsigned int ErrorCallbackCookie;
typedef unsigned int DebuggerAttachCallbackCookie;

enum class JSIEngine : int32_t {
  Chakra = 0, // Use the JSIExecutorFactory with ChakraRuntime
  Hermes = 1, // Use the JSIExecutorFactory with Hermes
  V8 = 2, // Use the JSIExecutorFactory with V8
};

struct ReactInstanceSettings {
  bool UseWebDebugger{false};
  bool UseFastRefresh{false};
  bool UseLiveReload{false};
  bool DebuggerBreakOnNextLine{false};
  bool UseDirectDebugger{false};
  bool UseJsi{true};
  bool EnableJITCompilation{true};
  bool EnableByteCodeCaching{false};
  bool EnableDeveloperMenu{false};
  uint16_t DebuggerPort{9229};

  std::string ByteCodeFileUri;
  std::string DebugHost;
  std::string DebugBundlePath;
  std::string BundleRootPath;
  facebook::react::NativeLoggingHook LoggingCallback;
  std::shared_ptr<Mso::React::IRedBoxHandler> RedBoxHandler;
#if defined (USE_HERMES)
  JSIEngine jsiEngine{JSIEngine::Hermes};
#else
  JSIEngine jsiEngine{JSIEngine::Chakra};
#endif
};

struct IReactInstance {
  virtual ~IReactInstance() = default;

  virtual void AttachMeasuredRootView(IXamlRootView *pRootView, folly::dynamic &&initProps) = 0;
  virtual void DetachRootView(IXamlRootView *pRootView) = 0;

  virtual LiveReloadCallbackCookie RegisterLiveReloadCallback(std::function<void()> callback) = 0;
  virtual void UnregisterLiveReloadCallback(LiveReloadCallbackCookie &cookie) = 0;

  virtual ErrorCallbackCookie RegisterErrorCallback(std::function<void()> callback) = 0;
  virtual void UnregisterErrorCallback(ErrorCallbackCookie &cookie) = 0;

  virtual DebuggerAttachCallbackCookie RegisterDebuggerAttachCallback(std::function<void()> callback) = 0;
  virtual void UnRegisterDebuggerAttachCallback(DebuggerAttachCallbackCookie &cookie) = 0;

  virtual void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) = 0;

  virtual void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept = 0;

  virtual const std::shared_ptr<facebook::react::MessageQueueThread> &JSMessageQueueThread() const noexcept = 0;
  virtual const std::shared_ptr<facebook::react::MessageQueueThread> &DefaultNativeMessageQueueThread() const
      noexcept = 0;
  virtual facebook::react::INativeUIManager *NativeUIManager() const noexcept = 0;

  // Returns a flag that indicates some properties of the existing instance has
  // been changed (like using web debugger), and this instance should not be
  // cached and served anymore.
  virtual bool NeedsReload() const noexcept = 0;

  // Marks this instance as needing reload, due to a fundamental property change
  // (like useWebDebugger).
  virtual void SetAsNeedsReload() noexcept = 0;

  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept = 0;

  virtual bool IsInError() const noexcept = 0;
  virtual bool IsWaitingForDebugger() const noexcept = 0;
  virtual const std::string &LastErrorMessage() const noexcept = 0;

  virtual void loadBundle(std::string &&jsBundleRelativePath) = 0;

  // Returns the root path of the JS bundle. This is needed for
  // classes that do not have access to the settings object.
  virtual std::string GetBundleRootPath() const noexcept = 0;

  // Test Hooks
  virtual void SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> testHook) = 0;
  virtual void CallXamlViewCreatedTestHook(react::uwp::XamlView view) = 0;

  virtual ExpressionAnimationStore &GetExpressionAnimationStore() = 0;

  virtual bool IsLoaded() const noexcept = 0;
};

} // namespace uwp
} // namespace react
