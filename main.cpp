#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map>

typedef struct {
    std::string id;
    std::string brand;
    float mileage;
    int year;
} Car;

void printUsage();

int handleTxt2Bin(const std::string &srcPath, const std::string &dstPath);

int handleBin2Txt(const std::string &srcPath, const std::string &dstPath);

int handlePrintTxt(const std::string &txtFilePath);

int handlePrintBin(const std::string &binFilePath);

int handleMoreThan100000(const std::string &binFilePath);

int handleNewest5(const std::string &binFilePath);

int handleCreateRow(const std::string &binFilePath, const std::string &id, const std::string &brand, float mileage,
                    int year);

int handleUpdateRow(const std::string &binFilePath, int row, const std::string &brand, float mileage);

int handleUpdateRow(const std::string &binFilePath, int row, const std::string &brand);

int handleUpdateRow(const std::string &binFilePath, int row, float mileage);

int handleDeleteRow(const std::string &binFilePath, int row);

int readTxtFile(std::vector<Car *> &out, std::istream &is);

Car *readCarTxt(int iLine, const std::string &line);

int readBinFile(std::vector<Car *> &out, std::istream &is);

Car *readCarBin(std::istream &is);

int writeBinFile(std::vector<Car *> &cars, std::ostream &os);

int writeCarBin(std::ostream &os, const Car &car);

int writeTxtFile(std::vector<Car *> &cars, std::ostream &os);

int writeCarTxt(std::ostream &os, const Car &car);

void printCar(const Car &car);

void printCars(const std::vector<Car *> cars);

int main(int argc, char **argv) {
    /**
     * Usage:
     *     my_lab1 txt-2-bin <path_to_text_file> <path_to_bin_file>
     *     my_lab1 bin-2-text <path_to_bin_file> <path_to_text_file>
     *     my_lab1 print-text <path_to_text_file>
     *     my_lab1 print-bin <path_to_bin_file>
     *     my_lab1 more-100-000-bin <path_to_bin_file>
     *     my_lab1 5-newest-bin <path_to_bin_file>
     *     my_lab1 update-row <path_to_bin_file> <i_row> [key:value] ...
     *     my_lab1 delete-row <path_to_bin_file> <i_row>
     *     my_lab1 create-row <path_to_bin_file> [key:value] ...
     */

    if (argc < 3) {
        printUsage();
        return 1;
    }

    std::string cmd(argv[1]);
    if (cmd == "txt-2-bin") {
        if (argc < 4) {
            printUsage();
            return 1;
        }
        std::string srcPath(argv[2]);
        std::string dstPath(argv[3]);
        int res = handleTxt2Bin(srcPath, dstPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "bin-2-txt") {
        if (argc < 4) {
            printUsage();
            return 1;
        }
        std::string srcPath(argv[2]);
        std::string dstPath(argv[3]);
        int res = handleBin2Txt(srcPath, dstPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "print-txt") {
        if (argc < 3) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);
        int res = handlePrintTxt(srcPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "print-bin") {
        if (argc < 3) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);
        int res = handlePrintBin(srcPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "more-100-000-bin") {
        if (argc < 3) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);
        int res = handleMoreThan100000(srcPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "5-newest-bin") {
        if (argc < 3) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);
        int res = handleNewest5(srcPath);
        if (res != 0) {
            return 1;
        }
    } else if (cmd == "update-row") {
        if (argc < 4) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);

        std::string strRow(argv[3]);
        std::stringstream rowStream(strRow);
        int row;
        rowStream >> row;
        if (rowStream.bad() || rowStream.fail()) {
            printUsage();
            return 1;
        }

        size_t semicolonPos;
        std::map<std::string, std::string> params;

        for (int i = 4; i < argc; i++) {
            std::string kv(argv[i]);
            semicolonPos = kv.find(":");
            if (semicolonPos == std::string::npos) {
                printUsage();
                return 1;
            }
            params[kv.substr(0, semicolonPos)] = kv.substr(semicolonPos + 1, kv.size() - semicolonPos - 1);
        }

        if (params.count("mileage") == 1 && params.count("brand")) {
            float mileage;
            std::stringstream mileageStream(params["mileage"]);
            mileageStream >> mileage;
            if (mileageStream.bad() || mileageStream.fail()) {
                printUsage();
                return 1;
            }

            int res = handleUpdateRow(srcPath, row, params["brand"], mileage);
            if (res != 0) {
                return 1;
            }
        } else if (params.count("mileage") == 1) {
            float mileage;
            std::stringstream mileageStream(params["mileage"]);
            mileageStream >> mileage;
            if (mileageStream.bad() || mileageStream.fail()) {
                printUsage();
                return 1;
            }

            int res = handleUpdateRow(srcPath, row, mileage);
            if (res != 0) {
                return 1;
            }
        } else if (params.count("brand") == 1) {
            int res = handleUpdateRow(srcPath, row, params["brand"]);
            if (res != 0) {
                return 1;
            }
        }
    } else if (cmd == "create-row") {
        if (argc < 7) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);

        size_t semicolonPos;
        std::map<std::string, std::string> params;

        for (int i = 3; i < argc; i++) {
            std::string kv(argv[i]);
            semicolonPos = kv.find(":");
            if (semicolonPos == std::string::npos) {
                printUsage();
                return 1;
            }
            params[kv.substr(0, semicolonPos)] = kv.substr(semicolonPos + 1, kv.size() - semicolonPos - 1);
        }

        if (params.count("id") != 1) {
            printUsage();
            return 1;
        }

        if (params.count("brand") != 1) {
            printUsage();
            return 1;
        }

        if (params.count("mileage") != 1) {
            printUsage();
            return 1;
        }

        if (params.count("year") != 1) {
            printUsage();
            return 1;
        }

        std::stringstream mileageStream(params["mileage"]);
        float mileage;
        mileageStream >> mileage;
        if (mileageStream.bad() || mileageStream.fail()) {
            printUsage();
            return 1;
        }

        std::stringstream yearStream(params["year"]);
        int year;
        yearStream >> year;
        if (mileageStream.bad() || mileageStream.fail()) {
            printUsage();
            return 1;
        }

        handleCreateRow(srcPath, params["id"], params["brand"], mileage, year);
    } else if (cmd == "delete-row") {
        if (argc < 4) {
            printUsage();
            return 1;
        }

        std::string srcPath(argv[2]);
        std::string strRow(argv[3]);
        std::stringstream rowStream(strRow);
        int row;
        rowStream >> row;
        if (rowStream.bad() || rowStream.fail()) {
            printUsage();
            return 1;
        }

        int res = handleDeleteRow(srcPath, row);
        if (res != 0) {
            return 1;
        }
    } else {
        printUsage();
        return 1;
    }

    return 0;
}

void printUsage() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "    my_lab1 txt-2-bin <path_to_text_file> <path_to_bin_file>" << std::endl;
    std::cerr << "    my_lab1 bin-2-text <path_to_bin_file> <path_to_text_file>" << std::endl;
    std::cerr << "    my_lab1 print-text <path_to_text_file>" << std::endl;
    std::cerr << "    my_lab1 print-bin <path_to_bin_file>" << std::endl;
    std::cerr << "    my_lab1 more-100-000-bin <path_to_bin_file>" << std::endl;
    std::cerr << "    my_lab1 5-newest-bin <path_to_bin_file>" << std::endl;
    std::cerr << "    my_lab1 update-row <path_to_bin_file> <i_row> [key:value] ..." << std::endl;
    std::cerr << "    my_lab1 delete-row <path_to_bin_file> <i_row>" << std::endl;
    std::cerr << "    my_lab1 create-row <path_to_bin_file> <i_row> [key:value] ..." << std::endl;
}

int handleTxt2Bin(const std::string &srcPath, const std::string &dstPath) {
    std::vector<Car *> cars;

    std::ifstream ifs(srcPath);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << srcPath << std::endl;
        return 1;
    }

    if (readTxtFile(cars, ifs)) {
        std::cerr << "Can not read file " << srcPath << std::endl;
        return 1;
    }

    printCars(cars);

    std::ofstream ofs(dstPath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << dstPath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleBin2Txt(const std::string &srcPath, const std::string &dstPath) {
    std::vector<Car *> cars;

    std::ifstream ifs(srcPath, std::ios_base::binary | std::ios_base::in);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << srcPath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << srcPath << std::endl;
        return 1;
    }

    printCars(cars);

    std::ofstream ofs(dstPath);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << dstPath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeTxtFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    for (auto &car : cars) {
        delete car;
    }
    return 0;
}

int handlePrintTxt(const std::string &txtFilePath) {
    std::vector<Car *> cars;

    std::ifstream ifs(txtFilePath);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << txtFilePath << std::endl;
        return 1;
    }

    if (readTxtFile(cars, ifs)) {
        std::cerr << "Can not read file " << txtFilePath << std::endl;
        return 1;
    }

    printCars(cars);

    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handlePrintBin(const std::string &binFilePath) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }

    printCars(cars);

    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleMoreThan100000(const std::string &binFilePath) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }

    std::vector<Car *> filtered;
    for (auto &i : cars) {
        if (i->mileage > 100000) {
            filtered.push_back(i);
        }
    }
    printCars(filtered);

    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleNewest5(const std::string &binFilePath) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }

    std::sort(cars.begin(), cars.end(), [](Car *a, Car *b) {
        return a->year > b->year;
    });
    if (cars.size() > 5) {
        std::for_each(cars.begin() + 5, cars.end(), [](Car *i) { delete i; });
        cars.erase(cars.begin() + 5, cars.end());
    }
    printCars(cars);

    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleCreateRow(const std::string &binFilePath, const std::string &id, const std::string &brand, float mileage,
                    int year) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }
    ifs.close();

    Car *car = new Car();
    car->id = id;
    car->brand = brand;
    car->mileage = mileage;
    car->year = year;
    cars.push_back(car);

    printCars(cars);

    std::ofstream ofs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }
    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleDeleteRow(const std::string &binFilePath, int row) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }
    ifs.close();

    if (row >= cars.size()) {
        std::cerr << "Index out of bounds : " << row << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    delete cars[row];
    cars.erase(cars.begin() + row, cars.begin() + row + 1);

    printCars(cars);

    std::ofstream ofs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }
    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleUpdateRow(const std::string &binFilePath, int row, const std::string &brand, float mileage) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }
    ifs.close();

    if (row >= cars.size()) {
        std::cerr << "Index out of bounds : " << row << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    Car *car = cars[row];
    car->brand = brand;
    car->mileage = mileage;

    printCars(cars);

    std::ofstream ofs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }
    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleUpdateRow(const std::string &binFilePath, int row, const std::string &brand) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }
    ifs.close();

    if (row >= cars.size()) {
        std::cerr << "Index out of bounds : " << row << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    Car *car = cars[row];
    car->brand = brand;

    printCars(cars);

    std::ofstream ofs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }
    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

int handleUpdateRow(const std::string &binFilePath, int row, float mileage) {
    std::vector<Car *> cars;

    std::ifstream ifs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ifs.fail() || ifs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        return 1;
    }

    if (readBinFile(cars, ifs)) {
        std::cerr << "Can not read file " << binFilePath << std::endl;
        return 1;
    }
    ifs.close();

    if (row >= cars.size()) {
        std::cerr << "Index out of bounds : " << row << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    Car *car = cars[row];
    car->mileage = mileage;

    printCars(cars);

    std::ofstream ofs(binFilePath, std::ios_base::binary | std::ios_base::out);
    if (ofs.fail() || ofs.bad()) {
        std::cerr << "Can not open file " << binFilePath << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }

    if (writeBinFile(cars, ofs)) {
        std::cerr << "Can not write car" << std::endl;
        for (auto &car : cars) {
            delete car;
        }
        return 1;
    }
    for (auto &car : cars) {
        delete car;
    }

    return 0;
}

void printCar(const Car &car) {
    std::cout << std::setw(8) << car.id << " "
              << std::setw(15) << car.brand << " "
              << std::setw(15) << std::fixed << std::setprecision(2) << car.mileage << " "
              << std::setw(7) << car.year << std::endl;
}

void printCars(const std::vector<Car *> cars) {
    std::cout << "   Реестр автомобилей" << std::endl;
    std::cout << "  --------------------------------------------------  " << std::endl;
    std::cout << std::setw(8) << std::right << "Id"
              << std::setw(16) << std::right << "Brand"
              << std::setw(16) << std::right << "Mileage"
              << std::setw(8) << std::right << "Year" << std::endl;
    for (auto &car : cars) {
        printCar(*car);
    }
}

int readTxtFile(std::vector<Car *> &out, std::istream &is) {
    std::string line;
    bool error = false;
    int iLine = 0;
    while (std::getline(is, line)) {
        Car *car = readCarTxt(iLine++, line);
        if (car != nullptr) {
            out.push_back(car);
        } else {
            error = true;
        }
    }

    if (error) {
        for (auto &car : out) {
            delete car;
        }
        out.clear();
        return 1;
    }

    return 0;
}

Car *readCarTxt(int iLine, const std::string &line) {
    std::stringstream lineStream(line);
    std::string id;
    std::string brand;
    float mileage;
    int year;

    lineStream >> id >> brand >> mileage >> year;
    if (lineStream.fail() || lineStream.bad()) {
        std::cerr << "Wrong string format [" << iLine << "]" << std::endl;
        return nullptr;
    }

    if (id.size() != 8
        || !((id[0] >= 'A' && id[0] <= 'Z') || (id[0] >= 'a' && id[0] <= 'z'))
        || !((id[1] >= 'A' && id[1] <= 'Z') || (id[1] >= 'a' && id[1] <= 'z'))
        || !((id[2] >= 'A' && id[2] <= 'Z') || (id[2] >= 'a' && id[2] <= 'z'))
        || !(id[3] >= '0' && id[3] <= '9')
        || !(id[4] >= '0' && id[4] <= '9')
        || !(id[5] >= '0' && id[5] <= '9')
        || !(id[6] >= '0' && id[6] <= '9')
        || !(id[7] >= '0' && id[7] <= '9')
            ) {
        std::cerr << "Wrong id format [" << iLine << "]" << std::endl;
        return nullptr;
    }

    Car *car = new Car();
    car->id = id;
    car->brand = brand;
    car->mileage = mileage;
    car->year = year;
    return car;
}

int readBinFile(std::vector<Car *> &out, std::istream &is) {
    while (!is.eof()) {
        Car *car = readCarBin(is);
        if (car == nullptr) {
            if (is.eof()) {
                break;
            } else {
                for (auto &car : out) {
                    delete car;
                }
            }
            out.clear();
            return 1;
        } else {
            out.push_back(car);
        }
    }
    return 0;
}

Car *readCarBin(std::istream &is) {
    unsigned long idSize;
    is.read((char *) &idSize, sizeof(idSize));
    if (is.eof() || is.fail() || is.bad()) {
        return nullptr;
    }

    char *id = new char[idSize + 1];
    is.read(id, idSize);
    if (is.fail() || is.bad() || is.eof()) {
        delete[]id;
        return nullptr;
    }
    id[idSize] = 0;

    unsigned long brandSize;
    is.read((char *) &brandSize, sizeof(brandSize));
    char *brand = new char[brandSize + 1];
    is.read(brand, brandSize);
    if (is.fail() || is.bad() || is.eof()) {
        delete[]id;
        delete[]brand;
        return nullptr;
    }
    brand[brandSize] = 0;

    float mileage;
    is.read((char *) &mileage, sizeof(mileage));
    if (is.fail() || is.bad() || is.eof()) {
        delete[]id;
        delete[]brand;
        return nullptr;
    }

    int year;
    is.read((char *) &year, sizeof(year));
    if (is.fail() || is.bad() || is.eof()) {
        delete[]id;
        delete[]brand;
        return nullptr;
    }

    Car *car = new Car();
    car->id = std::string(id);
    car->brand = std::string(brand);
    car->mileage = mileage;
    car->year = year;
    return car;
}

int writeBinFile(std::vector<Car *> &cars, std::ostream &os) {
    for (auto &car : cars) {
        if (writeCarBin(os, *car)) {
            return 1;
        }
    }

    return 0;
}

int writeCarBin(std::ostream &os, const Car &car) {
    unsigned long idSize = car.id.size();
    unsigned long brandSize = car.brand.size();
    float mileage = car.mileage;
    int year = car.year;
    os.write((const char *) &idSize, sizeof(idSize));
    os.write(car.id.c_str(), idSize);
    os.write((const char *) &brandSize, sizeof(brandSize));
    os.write(car.brand.c_str(), brandSize);
    os.write((const char *) &mileage, sizeof(mileage));
    os.write((const char *) &year, sizeof(year));
    return os.fail() || os.bad();
}

int writeTxtFile(std::vector<Car *> &cars, std::ostream &os) {
    for (auto &car : cars) {
        if (writeCarTxt(os, *car)) {
            return 1;
        }
    }

    return 0;
}

int writeCarTxt(std::ostream &os, const Car &car) {
    os << std::setw(8) << car.id << " "
       << std::setw(15) << car.brand << " "
       << std::setw(15) << std::fixed << std::setprecision(2) << car.mileage << " "
       << std::setw(7) << car.year << std::endl;
    if (os.fail() || os.bad()) {
        return 1;
    } else {
        return 0;
    }
}
