#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>
#include <vector>
#include <utility>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <QSysInfo>
#include <algorithm>
#include <QCryptographicHash>
#include <QDebug>



QString calculateHash(const QString& data, QCryptographicHash::Algorithm algorithm) {
    /**
     * Calculate the cryptographic hash of a string using the specified algorithm.
     *
     * @param data Input string to hash
     * @param algorithm Hash algorithm (e.g., QCryptographicHash::Sha256)
     *
     * @return Hexadecimal string of the hash digest
     */

    // Convert QString to UTF-8 encoded byte array
    QByteArray byteData = data.toUtf8();

    // Calculate hash
    QCryptographicHash hash(algorithm);
    hash.addData(byteData);
    QByteArray hashResult = hash.result();

    // Return hexadecimal string
    return QString(hashResult.toHex());
}


void encode_uuid_to_exe(const std::string& exe_path, const std::string& uuid) {
    std::ifstream exe_file_in(exe_path, std::ios::binary);
    if (!exe_file_in) {
        // std::cerr << "Failed to open executable for reading." << std::endl;
        return;
    }

    // Read entire binary
    std::vector<char> binary_data((std::istreambuf_iterator<char>(exe_file_in)),
                                  std::istreambuf_iterator<char>());
    exe_file_in.close();

    // Find placeholder
    std::string placeholder = "__UUID_PLACEHOLDER__";
    auto pos = std::search(binary_data.begin(), binary_data.end(),
                           placeholder.begin(), placeholder.end());

    if (pos != binary_data.end()) {
        // Replace placeholder with UUID
        std::copy(uuid.begin(), uuid.end(), pos);

        // Write back to executable
        std::ofstream exe_file_out(exe_path, std::ios::binary);
        exe_file_out.write(binary_data.data(), binary_data.size());
        exe_file_out.close();

        // std::cout << "UUID successfully encoded into executable." << std::endl;
    } else {
        // std::cerr << "UUID placeholder not found in executable." << std::endl;
    }
}



class RegistrationForm : public QWidget {
    Q_OBJECT

public:
    RegistrationForm(QWidget *parent = nullptr)
        : QWidget(parent) {
        // 设置窗口属性
        // setWindowTitle("用户注册");
        setWindowTitle("Registration");
        setFixedSize(400, 300);

        // 创建主布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(15);
        mainLayout->setContentsMargins(30, 20, 30, 30);

        // 标题
        // QLabel *titleLabel = new QLabel("用户注册");
        QLabel *titleLabel = new QLabel("User Registration");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
        mainLayout->addWidget(titleLabel);

        // 添加间距
        mainLayout->addSpacing(20);

        // 用户名输入区域
        // QLabel *usernameLabel = new QLabel("用户名:");
        QLabel *usernameLabel = new QLabel("User:");
        usernameInput = new QLineEdit;
        usernameInput->setPlaceholderText("user name please");
        // usernameInput->setPlaceholderText("请输入用户名");
        usernameInput->setClearButtonEnabled(true);
        usernameInput->setFixedHeight(30);

        QVBoxLayout *usernameLayout = new QVBoxLayout;
        usernameLayout->addWidget(usernameLabel);
        usernameLayout->addWidget(usernameInput);
        mainLayout->addLayout(usernameLayout);

        // 密码输入区域
        // QLabel *passwordLabel = new QLabel("密码:");
        QLabel *passwordLabel = new QLabel("Password:");
        passwordInput = new QLineEdit;
        // passwordInput->setPlaceholderText("请输入密码");
        passwordInput->setPlaceholderText("password please");
        passwordInput->setEchoMode(QLineEdit::Password);
        passwordInput->setClearButtonEnabled(true);
        passwordInput->setFixedHeight(30);

        QVBoxLayout *passwordLayout = new QVBoxLayout;
        passwordLayout->addWidget(passwordLabel);
        passwordLayout->addWidget(passwordInput);
        mainLayout->addLayout(passwordLayout);

        // 添加间距
        mainLayout->addSpacing(30);

        // 注册按钮
        // registerButton = new QPushButton("注册");
        registerButton = new QPushButton("Go");
        registerButton->setFixedHeight(40);
        registerButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #4CAF50;"
            "   color: white;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #45a049;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #3d8b40;"
            "}"
            );

        mainLayout->addWidget(registerButton);

        // 连接信号槽
        connect(registerButton, &QPushButton::clicked, this, &RegistrationForm::registerUser);

        users.push_back(std::make_pair("123456", "cbca4318ed6e0578e571d5a1962c2cc99251cd86d3a4fbe550bc7d0c60af16cb"));
    }

private slots:
    void registerUser() {
        QString username = usernameInput->text().trimmed();
        QString password = passwordInput->text().trimmed();

        if (username.isEmpty() || password.isEmpty()) {
            // QMessageBox::warning(this, "输入错误", "用户名和密码不能为空！");
            QMessageBox::warning(this, "Input Error", "User or Password cannot be null！");
            return;
        }



        // 检查用户名是否已存在（硬编码列表）
        bool usernameExists = false;
        for (const auto& user : users) {
            QString sha256Hash = calculateHash(user.second, QCryptographicHash::Sha256);
            if (user.first == username && calculateHash(password, QCryptographicHash::Sha256) == user.second) {
                usernameExists = true;
                break;
            }
        }

        if (usernameExists) {
            // read uuid
            QByteArray id = QSysInfo::machineUniqueId();
            // write uuid
            encode_uuid_to_exe("GenerateMask.exe", id.toHex().toStdString());
            QMessageBox::warning(this, "Info", "Success！");
            usernameInput->clear();
            passwordInput->clear();

            return;
        }
        else{
            QMessageBox::warning(this, "Info", "Fail, Contact the Author！");
        }

        // 添加到硬编码用户列表
        // users.push_back(std::make_pair(username, password));

        // QMessageBox::information(
        //     this,
        //     "注册成功",
        //     QString("用户 %1 注册成功！\n\n当前用户数: %2").arg(username).arg(users.size())
        //     );

        // 清空输入框
        usernameInput->clear();
        passwordInput->clear();

        // 打印所有用户（调试用）
        // printAllUsers();
    }

private:
    // 打印所有用户（用于演示）
    void printAllUsers() {
        qDebug() << "\n====== 当前注册用户 ======";
        for (const auto& user : users) {
            qDebug() << "用户名:" << user.first << "密码:" << user.second;
        }
        qDebug() << "=========================\n";
    }

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QPushButton *registerButton;

    // 硬编码存储用户名和密码
    std::vector<std::pair<QString, QString>> users;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 设置应用样式
    QApplication::setStyle("Fusion");

    RegistrationForm window;
    window.show();

    return app.exec();
}

#include "main.moc"  // 处理Qt元对象系统
