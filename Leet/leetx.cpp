// https://leetcode.com/problems/add-two-numbers/submissions/1582212232/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution { // 4ms
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        int forward = 0;
        return __addTwoNumbers(l1, l2, forward);
    }
private:
    ListNode* __addTwoNumbers(ListNode* l1, ListNode* l2, int forward) {
        if (l1 == nullptr && l2 == nullptr && forward == 0) 
            return nullptr;

        ListNode *nl1 = nullptr, *nl2 = nullptr;
        int vl3 = forward;
        if (l1 != nullptr) {
            vl3 += l1->val;
            nl1 = l1->next;
        }
        if (l2 != nullptr) {
            vl3 += l2->val;
            nl2 = l2->next;
        }

        return new ListNode(vl3 % 10, __addTwoNumbers(nl1, nl2, vl3 / 10));
    }
};


class Solution2 { // 1ms
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode dummy(0);
        ListNode* tail = &dummy;
        int carry = 0;

        while (l1 || l2 || carry) {
            int sum = carry;
            if (l1) { sum += l1->val; l1 = l1->next; }
            if (l2) { sum += l2->val; l2 = l2->next; }

            carry = sum / 10;
            tail = tail->next = new ListNode(sum % 10);
        }

        return dummy.next;
    }
};

// # Definition for singly-linked list.
// # class ListNode:
// #     def __init__(self, val=0, next=None):
// #         self.val = val
// #         self.next = next

// from typing import Optional

// class Solution:
//     def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
//         dummy = ListNode()
//         tail = dummy
//         carry = 0
//         while (l1 is not None or l2 is not None or carry > 0):
//             sum1 = carry
//             if l1 is not None:
//                 sum1 += l1.val
//                 l1 = l1.next
//             if l2 is not None:
//                 sum1 += l2.val
//                 l2 = l2.next

//             carry = sum1 // 10
//             tail.next = ListNode(sum1 % 10)
//             tail = tail.next

//         return dummy.next

// https://leetcode.com/problems/longest-substring-without-repeating-characters/submissions/1582256239/

// class Solution:
//     def lengthOfLongestSubstring(self, s: str) -> int:
//         char_set = set()
//         left = 0
//         max_length = 0

//         for right in range(len(s)):
//             while s[right] in char_set:
//                 char_set.remove(s[left])
//                 left += 1
//             char_set.add(s[right])
//             max_length = max(max_length, right - left + 1)

//         return max_length

// #include <unordered_set>

// class Solution {
// public:
//     int lengthOfLongestSubstring(string s) {
//         unordered_set<char> st;
//         int left = 0, longest = 0;
//         for (int i = 0; i < s.size(); i++) {
//             while (st.count(s[i])) {
//                 st.erase(s[left]);
//                 left += 1;
//             }
//             st.insert(s[i]);
//             longest = max(longest, i - left + 1);
//         }
//         return longest;
//     }
// };

// https://leetcode.com/problems/median-of-two-sorted-arrays/submissions/1582289302/

// class Solution {
// public:
//     double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
//         int p1 = 0, p2 = 0, iter = 0;
        
//         const int num1_size = (int)nums1.size(), num2_size = (int)nums2.size();
        
//         const int median_idx = (num1_size + num2_size) / 2;
//         const bool check_next = (num1_size + num2_size) % 2 == 0;
        
//         int current_value1 = INT_MIN, current_value2 = INT_MIN;

//         while (true) {
//             current_value2 = current_value1;
//             if (p2 >= num2_size || ((p1 < num1_size) && (nums1[p1] < nums2[p2]))) {
//                 current_value1 = nums1[p1++];
//             } 
//             else {
//                 current_value1 = nums2[p2++];
//             }

//             if (median_idx == iter && !check_next) {
//                 return (double)current_value1;
//             }  
//             else if (median_idx == iter && check_next) {
//                 return (double)(current_value1 + current_value2) / 2.0;
//             }
//             iter += 1;
//         }
//     }
// };

// class Solution {
// public:
//     double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
//         int p1 = 0, p2 = 0;
//         int num1_size = nums1.size(), num2_size = nums2.size();
//         int median_idx = (num1_size + num2_size) / 2;
//         bool is_even = (num1_size + num2_size) % 2 == 0;

//         int current = 0, prev = 0;
//         for (int i = 0; i <= median_idx; i++) {
//             prev = current;
//             if (p2 >= num2_size || (p1 < num1_size && nums1[p1] < nums2[p2])) {
//                 current = nums1[p1++];
//             } else {
//                 current = nums2[p2++];
//             }
//         }

//         return is_even ? (prev + current) / 2.0 : current;
//     }
// };

