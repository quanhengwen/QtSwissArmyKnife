﻿/*
 * Copyright (C) 2018-2019 wuuhii. All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project. The project is a open source project, you can get the source from:
 *     https://github.com/wuuhii/QtSwissArmyKnife
 *     https://gitee.com/wuuhii/QtSwissArmyKnife
 *
 * If you want to know more about the project, please join our QQ group(952218522).
 * In addition, the email address of the project author is wuuhii@outlook.com.
 * Welcome to bother.
 *
 * I write the comment in English, it's not because that I'm good at English,
 * but for "installing B".
 */
#include <QDebug>
#include <QPixmap>
#include <QDateTime>
#include <QSettings>
#include <QKeyEvent>
#include <QMetaEnum>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QIntValidator>
#include <QLoggingCategory>

#include "SAKDebugPage.hh"
#include "SAKDataFactory.hh"
#include "SAKChartManager.hh"
#include "SAKCRCInterface.hh"
#include "SAKOtherSettings.hh"
#include "SAKStatisticsManager.hh"
#include "DebugPageInputManager.hh"
#include "DebugPageOutputManager.hh"
#include "DebugPageDeviceManager.hh"
#include "HighlightSettingsWidget.hh"
#include "DebugPageMessageManager.hh"

#include "ui_SAKDebugPage.h"

#define MINI_READ_WRITE_WATINGT_TIME 5   // 读写等待最小时间(单位为：ms)

SAKDebugPage::SAKDebugPage(QWidget *parent)
    :QWidget(parent)
    ,ui (new Ui::SAKDebugPage)
{
    ui->setupUi(this);
    initUiPointer();

    chartManager            = new SAKChartManager(this, this);
    outputManager           = new DebugPageOutputManager(this, this);
    otherSettings           = new SAKOtherSettings(this, this);
    statisticsManager       = new SAKStatisticsManager(this);
    debugPageDeviceManager  = new DebugPageDeviceManager(this, this);
    debugPageMessageManager = new DebugPageMessageManager(this, this);
    debugPageInputManager   = new DebugPageInputManager(this, this);

    _readWriteParameters.waitForReadyReadTime = MINI_READ_WRITE_WATINGT_TIME;
    _readWriteParameters.waitForBytesWrittenTime = MINI_READ_WRITE_WATINGT_TIME;

    connect(this, &SAKDebugPage::deviceStatusChanged, this, &SAKDebugPage::changedDeviceStatus);    
}

SAKDebugPage::~SAKDebugPage()
{
    delete ui;
    delete logCategory;
}

void SAKDebugPage::write(QByteArray data)
{
    emit writeDataRequest(data);
}

void SAKDebugPage::outputMessage(QString msg, bool isInfo)
{
    debugPageMessageManager->outputMessage(msg, isInfo);
}

struct SAKDebugPage::ReadWriteParameters SAKDebugPage::readWriteParameters()
{
    readWriteParametersQMutex.lock();
    ReadWriteParameters parameters;
    parameters.waitForReadyReadTime = _readWriteParameters.waitForReadyReadTime;
    parameters.waitForBytesWrittenTime = _readWriteParameters.waitForBytesWrittenTime;
    readWriteParametersQMutex.unlock();

    return  _readWriteParameters;
}

void SAKDebugPage::setReadWriteParameters(struct ReadWriteParameters parameters)
{
    if (parameters.waitForReadyReadTime < MINI_READ_WRITE_WATINGT_TIME){
        parameters.waitForReadyReadTime = MINI_READ_WRITE_WATINGT_TIME;
    }

    if (parameters.waitForBytesWrittenTime < MINI_READ_WRITE_WATINGT_TIME){
        parameters.waitForBytesWrittenTime = MINI_READ_WRITE_WATINGT_TIME;
    }

    readWriteParametersQMutex.lock();
    _readWriteParameters.waitForReadyReadTime = parameters.waitForReadyReadTime;
    _readWriteParameters.waitForBytesWrittenTime = parameters.waitForBytesWrittenTime;
    readWriteParametersQMutex.unlock();
}


void SAKDebugPage::openOrColoseDevice()
{

}

void SAKDebugPage::refreshDevice()
{

}

QWidget *SAKDebugPage::controllerWidget()
{
    return nullptr;
}

void SAKDebugPage::setUpController()
{
    QWidget *controller = controllerWidget();
    if (controller){
        QHBoxLayout *layout = new QHBoxLayout(controllerFrame);
        layout->addWidget(controller);
        layout->setMargin(0);
        controllerFrame->setLayout(layout);
    }
}

void SAKDebugPage::changedDeviceStatus(bool opened)
{
    sendPushButton->setEnabled(opened);
    if (opened){

    }else{

    }
}

void SAKDebugPage::initUiPointer()
{
    /*
     * 设备管理组
     */
    switchPushButton        = ui->switchPushButton;
    refreshPushButton       = ui->refreshPushButton;
    deviceSettingGroupBox   = ui->deviceSettingGroupBox;
    controllerFrame         = ui->controllerFrame;

    /*
     * 消息输出组
     */
    messageTextBrowser      = ui->messageTextBrowser;

    /*
     * 输入设置组
     */
    inputModelComboBox      = ui->inputModelComboBox;
    cycleEnableCheckBox     = ui->cycleEnableCheckBox;
    cycleTimeLineEdit       = ui->cycleTimeLineEdit;
    saveInputDataPushButton = ui->saveInputDataPushButton;
    readinFilePushButton    = ui->readinFilePushButton;
    addCRCCheckBox          = ui->addCRCCheckBox;
    bigeEndianCheckBox      = ui->bigeEndianCheckBox;
    clearInputPushButton    = ui->clearInputPushButton;
    sendPushButton          = ui->sendPushButton;
    inputTextEdit           = ui->inputTextEdit;
    crcParameterModelsComboBox = ui->crcParameterModelsComboBox;
    crcLabel                = ui->crcLabel;

    /*
     * 图表
     */
    throughputPushButton    = ui->throughputPushButton;
    moreChartPushButton     = ui->moreChartPushButton;

    /*
     * 数据统计
     */
    rxSpeedLabel            = ui->rxSpeedLabel;
    txSpeedLabel            = ui->txSpeedLabel;
    rxFramesLabel           = ui->rxFramesLabel;
    txFramesLabel           = ui->txFramesLabel;
    rxBytesLabel            = ui->rxBytesLabel;
    txBytesLabel            = ui->txBytesLabel;
    resetTxCountPushButton  = ui->resetTxCountPushButton;
    resetRxCountPushButton  = ui->resetRxCountPushButton;

    /*
     * 输出设置组
     */
    rxLabel                 = ui->rxLabel;
    txLabel                 = ui->txLabel;
    outputModelComboBox     = ui->outputModelComboBox;
    autoWrapCheckBox        = ui->autoWrapCheckBox;
    showDateCheckBox        = ui->showDateCheckBox;
    showTimeCheckBox        = ui->showTimeCheckBox;
    showMsCheckBox          = ui->showMsCheckBox;
    showRxDataCheckBox      = ui->showRxDataCheckBox;
    showTxDataCheckBox      = ui->showTxDataCheckBox;
    saveOutputFileToFilecheckBox = ui->saveOutputFileToFilecheckBox;
    outputFilePathPushButton= ui->outputFilePathPushButton;
    clearOutputPushButton   = ui->clearOutputPushButton;
    saveOutputPushButton    = ui->saveOutputPushButton;
    outputTextBroswer       = ui->outputTextBroswer;
}
