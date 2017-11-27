#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

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

int readTxtFile(std::vector<Car *> &out, std::istream &is);

Car *readCarTxt(int iLine, const std::string &line);

int readBinFile(std::vector<Car *> &out, std::istream &is);

Car *readCarBin(std::istream &is);

int writeBinFile(std::vector<Car *> &cars, std::ostream &os);

int writeCarBin(std::ostream &os, const Car &car);

int writeTxtFile(std::vector<Car *> &cars, std::ostream &os);

int writeCarTxt(std::ostream &os, const Car &car);

void printCar(const Car &car);

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
     *     my_lab1 create-row <path_to_bin_file> <i_row> [key:value] ...
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

    std::cout << "   Реестр автомобилей" << std::endl;
    std::cout << "    Id         Brand          Mileage           Year" << std::endl;
    for (auto &car : cars) {
        printCar(*car);
    }

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

    std::cout << "   Реестр автомобилей" << std::endl;
    std::cout << "    Id         Brand          Mileage           Year" << std::endl;
    for (auto &car : cars) {
        printCar(*car);
    }

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

    return 0;
}

void printCar(const Car &car) {
    std::cout << car.id << " "
              << car.brand << " "
              << car.mileage << " "
              << car.year << std::endl;
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
    char *id = new char[idSize + 1];
    is.read(id, idSize);
    if (is.fail() || is.bad()) {
        delete[]id;
        return nullptr;
    }
    id[idSize] = 0;

    unsigned long brandSize;
    is.read((char *) &brandSize, sizeof(brandSize));
    char *brand = new char[brandSize + 1];
    is.read(brand, brandSize);
    if (is.fail() || is.bad()) {
        delete[]id;
        delete[]brand;
        return nullptr;
    }
    brand[brandSize] = 0;

    float mileage;
    is.read((char *) &mileage, sizeof(mileage));
    if (is.fail() || is.bad()) {
        delete[]id;
        delete[]brand;
        return nullptr;
    }

    int year;
    is.read((char *) &year, sizeof(year));
    if (is.fail() || is.bad()) {
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
    os << car.id << " "
       << car.brand << " "
       << car.mileage << " "
       << car.year << std::endl;
    if (os.fail() || os.bad()) {
        return 1;
    } else {
        return 0;
    }
}
