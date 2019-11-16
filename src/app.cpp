/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 Ray Andrew
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "app.h"

namespace emmerich {
class AppImpl : public App {
 private:
  const std::unique_ptr<QApplication>                 _qApp;
  const std::unique_ptr<ui::MainWindow>               _window;
  Config*                                             _config;
  State*                                              _state;
  std::unique_ptr<Logger>                             _logger;
  std::unique_ptr<mechanisms::finger::FingerMovement> _fingerMovement;

 public:
  INJECT(
      AppImpl(ASSISTED(int) argc,
              ASSISTED(char**) argv,
              Config*                                   config,
              State*                                    state,
              LoggerFactory                             loggerFactory,
              mechanisms::finger::FingerMovementFactory fingerMovementFactory))
      : _qApp(std::make_unique<QApplication>(argc, argv)),
        _window(std::make_unique<ui::MainWindow>()),
        _config(config),
        _state(state),
        _logger(loggerFactory("App")),
        _fingerMovement(fingerMovementFactory(nullptr)) {
    _logger->info("Automated Tending Project v{}.{}.{}.{}",
                  PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR,
                  PROJECT_VERSION_PATCH, PROJECT_VERSION_TWEAK);
    // _fingerMovementFactoryProvider.get()->moveX(10);
    // QPushButton* tendingButton
    // =_window->findChild<QPushButton*>("tending_button");
    QPushButton* tendingButton = _window->getUi()->tending_button;
    QObject::connect(tendingButton, &QPushButton::clicked,
                     [=]() { tendingButton->setText("Hi"); });
    _window->show();
  }

  virtual ~AppImpl() { spdlog::drop_all(); }

  int run() override { return _qApp->exec(); }
};

fruit::Component<AppFactory> getAppComponent() {
  return fruit::createComponent()
      .bind<App, AppImpl>()
      .install(getConfigComponent)
      .install(getLoggerComponent)
      .install(getStateComponent)
      .install(mechanisms::finger::getFingerMovementComponent);
}
}  // namespace emmerich