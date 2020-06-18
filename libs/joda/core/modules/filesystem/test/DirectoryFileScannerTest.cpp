#include <gtest/gtest.h>
#include <joda/fs/DirectoryFileScanner.h>

class DirectoryFileScannerTest : public testing::Test {
    protected:
    const std::string jsonDir = "./test/data/json";
    const std::string dirTestDir = "./test/data/directorytest";
};

TEST_F(DirectoryFileScannerTest, GetAllFilesInDir) {
    joda::filesystem::DirectoryFileScanner scanner;
    auto dir = jsonDir;

    auto files = scanner.getFilesInDir(dir);

    EXPECT_EQ(files.size(), 2);

    ASSERT_TRUE(std::find(files.begin(),files.end(),dir+"/formattest.json") != files.end());
    ASSERT_TRUE(std::find(files.begin(),files.end(),dir+"/numtest.json") != files.end());
}

TEST_F(DirectoryFileScannerTest, GetAllFilesInDirTrailingSlash) {
    joda::filesystem::DirectoryFileScanner scanner;
    auto dir = jsonDir +"/";

    auto files = scanner.getFilesInDir(dir);

    EXPECT_EQ(files.size(), 2);

    ASSERT_TRUE(std::find(files.begin(),files.end(),dir+"formattest.json") != files.end());
    ASSERT_TRUE(std::find(files.begin(),files.end(),dir+"numtest.json") != files.end());
}

TEST_F(DirectoryFileScannerTest, GetFilteredFilesInDir) {
    joda::filesystem::DirectoryFileScanner scanner;

    auto files = scanner.getFilesInDir(dirTestDir);
    EXPECT_EQ(files.size(), 4);
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/a.json") != files.end());
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/b.xml") != files.end());
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/c.txt") != files.end());

    files = scanner.getFilesInDir(dirTestDir,"json");
    EXPECT_EQ(files.size(), 1);
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/a.json") != files.end());

    files = scanner.getFilesInDir(dirTestDir,"xml");
    EXPECT_EQ(files.size(), 1);
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/b.xml") != files.end());

    files = scanner.getFilesInDir(dirTestDir,"txt");
    EXPECT_EQ(files.size(), 2);
    ASSERT_TRUE(std::find(files.begin(),files.end(),dirTestDir+"/c.txt") != files.end());

    files = scanner.getFilesInDir(dirTestDir,"abs");
    EXPECT_EQ(files.size(), 0);
}


TEST_F(DirectoryFileScannerTest, GetAllFilesNonExistingDir) {
    joda::filesystem::DirectoryFileScanner scanner;
    auto dir = dirTestDir +"/doesnotexistdir";

    auto files = scanner.getFilesInDir(dir);

    EXPECT_EQ(files.size(), 0);
}


TEST_F(DirectoryFileScannerTest, FileExistsTest) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_TRUE(scanner.fileIsValid(dirTestDir+"/a.json"));
}

TEST_F(DirectoryFileScannerTest, FileNotExistsTest) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_FALSE(scanner.fileIsValid(dirTestDir+"/d.abc"));
}

TEST_F(DirectoryFileScannerTest, HasEnding) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_TRUE(scanner.hasEnding(dirTestDir+"/a.json","json"));
    EXPECT_FALSE(scanner.hasEnding(dirTestDir+"/a.json","xml"));
}


TEST_F(DirectoryFileScannerTest, FileSize) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_EQ(scanner.getFileSize(dirTestDir+"/a.json"),1);
    EXPECT_EQ(scanner.getFileSize(dirTestDir+"/b.xml"),0);
    EXPECT_EQ(scanner.getFileSize(dirTestDir+"/c.txt"),3);
    EXPECT_EQ(scanner.getFileSize(dirTestDir+"/recursive/file.txt"),16);
}

TEST_F(DirectoryFileScannerTest, DirSizeNoFilter) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_EQ(scanner.getDirectorySize(dirTestDir,false,""),4);
}

TEST_F(DirectoryFileScannerTest, DirSizeRecursiveNoFilter) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_EQ(scanner.getDirectorySize(dirTestDir,true,""),20);
}

TEST_F(DirectoryFileScannerTest, DirSizeFilter) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_EQ(scanner.getDirectorySize(dirTestDir,false,"txt"),3);
}

TEST_F(DirectoryFileScannerTest, DirSizeRecursiveFilter) {
    joda::filesystem::DirectoryFileScanner scanner;
    EXPECT_EQ(scanner.getDirectorySize(dirTestDir,true,"txt"),19);
}
