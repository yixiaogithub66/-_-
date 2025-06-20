#pragma once // 预处理指令，确保此头文件在一次编译中仅被包含一次，以防止因重复包含导致的编译错误。

// 函数声明：显示充值菜单界面并处理充值逻辑。
// 这个函数可能会提供不同的充值选项，用户选择后会更新其余额。
// @param balance: 玩家当前的余额，函数可能会修改这个值。
// @return int: 充值操作完成后，返回玩家的新余额。
int paymentMenu(int balance);