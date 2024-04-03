start GPP_EXAM_RELEASE.exe -x 30 -y 30 -s 5

start GPP_EXAM_RELEASE.exe -x 1000 -y 30 -s 6

start GPP_EXAM_RELEASE.exe -x 30 -y 600 -s 7

start GPP_EXAM_RELEASE.exe -x 1000 -y 600 -s 4
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js