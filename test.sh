#!/bin/bash
curl -i "http://127.0.0.1:9527/?query=Nation%20==%20\"a\""
curl -i "http://127.0.0.1:7259/?job=INSERT%20%20\"ChinA\"%20,\"agri\"%20,%20%20\"ttest\"%20"
curl -i "http://127.0.0.1:9527/?query=%20Nation%20==%20\"ChinA\"%20and%20Category%20==%20\"agri\"%20"
curl -i "http://127.0.0.1:7259/?job=INSERT%20%20\"A\",%20\"B\"%20,\"C\"%20"
curl -i "http://127.0.0.1:7259/?job=INSERT%20%20\"D\"%20,\"E\",%20\"F\"%20"
curl -i "http://127.0.0.1:9527/?query=%20Nation%20&=%20\"A\"%20or%20Category%20$=%20\"e\"%20"