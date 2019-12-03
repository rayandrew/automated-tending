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
AppImpl::AppImpl(int                   argc,
                 char**                argv,
                 Config*               config,
                 Logger*               logger,
                 State*                state,
                 Dispatcher*           dispatcher,
                 mechanisms::Movement* movement)
    : _qApp(std::make_unique<QApplication>(argc, argv)),
      _config(std::move(config)),
      _state(std::move(state)),
      _logger(std::move(logger)),
      _dispatcher(std::move(dispatcher)) {
  _ui = _window->getUi();

  setupLogger();
  setupSignalsAndSlots();
  setupServices();

  _window->setWindowState(Qt::WindowMaximized);
  _window->show();

  _logger->info("Automated Tending Project v{}.{}.{}.{}", PROJECT_VERSION_MAJOR,
                PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH,
                PROJECT_VERSION_TWEAK);

  _state->load();
}

AppImpl::~AppImpl() {}

void AppImpl::setupLogger() {
  _logger->getLogger()->sinks().push_back(_qSpdlog);

  for (const auto& log_level_name : SPDLOG_LEVEL_NAMES) {
    _ui->cmbox_log_level->addItem(log_level_name);
  }

  _ui->cmbox_log_level->setCurrentText(QString::fromStdString(
      fmt::format(spdlog::level::to_string_view(_logger->level()))));
  connect(_ui->cmbox_log_level, &QComboBox::currentTextChanged, this,
          [this](const QString& level) {
            _logger->set_level(spdlog::level::info);
            auto level_str = level.toStdString();
            _logger->info("Log level: " + level_str);
            _logger->set_level(spdlog::level::from_str(level_str));
          });
  connect(_qSpdlog.get(), &QSpdlog::newLogEntry, this, &App::addLogEntry);
  connect(_ui->btn_clear_log_output, &QPushButton::released,
          _ui->ptextedit_log_output, &QPlainTextEdit::clear);
}

void AppImpl::setupSignalsAndSlots() {
  QLCDNumber*   stateFingerPositionValueX = _ui->stateFingerPositionValueX;
  QLCDNumber*   stateFingerPositionValueY = _ui->stateFingerPositionValueY;
  QLCDNumber*   stateFingerDegreeValue = _ui->stateFingerDegreeValue;
  QProgressBar* progressBar = _ui->progressBar;
  QLabel*       stateMachineStatus = _ui->stateMachineStatus;

  progressBar->setRange(0, 100);

  connect(_state, SIGNAL(xHasChanged(QString)), stateFingerPositionValueX,
          SLOT(display(QString)));
  connect(_state, SIGNAL(yHasChanged(QString)), stateFingerPositionValueY,
          SLOT(display(QString)));
  connect(_state, SIGNAL(degreeHasChanged(QString)), stateFingerDegreeValue,
          SLOT(display(QString)));

  connect(_state, &State::progressHasChanged, progressBar,
          &QProgressBar::setValue);

  connect(_state, &State::machineStateStringHasChanged, stateMachineStatus,
          &QLabel::setText);
  connect(_state, &State::machineStateHasChanged, _dispatcher,
          &Dispatcher::handleTask);
}

void AppImpl::setupServices() {
  QPushButton* tendingButton = _ui->tendingButton;
  QPushButton* wateringButton = _ui->wateringButton;
  QPushButton* resetButton = _ui->resetButton;
  QPushButton* stopButton = _ui->stopButton;

  connect(tendingButton, &QPushButton::released, this,
          [=]() { _state->setMachineState(task_state::TENDING); });
  connect(wateringButton, &QPushButton::released, this,
          [=]() { _state->setMachineState(task_state::WATERING); });
  connect(resetButton, &QPushButton::released, this,
          [=]() { _state->setMachineState(task_state::RESET); });
  connect(stopButton, &QPushButton::released, this,
          [=]() { _state->setMachineState(task_state::STOP); });
}

void AppImpl::addLogEntry(const QString& msg) {
  auto line_count = _ui->ptextedit_log_output->document()->lineCount();
  auto msg_formatted =
      QString("%1 %2").arg(QString::number(line_count), 4).arg(msg);
  _ui->ptextedit_log_output->insertPlainText(msg_formatted);
  if (_ui->chkbox_log_autoscroll->isChecked()) {
    _ui->ptextedit_log_output->verticalScrollBar()->setValue(
        _ui->ptextedit_log_output->verticalScrollBar()->maximum());
  }
}

fruit::Component<AppFactory> getAppComponent() {
  return fruit::createComponent()
      .bind<App, AppImpl>()
      .install(getConfigComponent)
      .install(getLoggerComponent)
      .install(getStateComponent)
      .install(getDispatcherComponent)
      .install(mechanisms::getMovementMechanismComponent);
}
}  // namespace emmerich
