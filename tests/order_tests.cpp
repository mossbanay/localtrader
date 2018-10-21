#include <algorithm>
#include <vector>

#include "gtest/gtest.h"
#include "order.h"

using namespace exchange;

TEST(OrderTest, unfilled_by_default) {
    Client bob("bob");
    Order o("ABC", 50.00, 2, BUY, bob);
    ASSERT_EQ(UNFILLED, o.get_status());
}

TEST(OrderTest, can_change_order_status) {
    Client bob("bob");
    Order o("ABC", 50.00, 2, BUY, bob);

    o.set_status(PARTIALLY_FILLED);
    ASSERT_EQ(PARTIALLY_FILLED, o.get_status());

    o.set_status(CANCELLED);
    ASSERT_EQ(CANCELLED, o.get_status());
}

TEST(OrderTest, price_order_respected) {
    Client bob("bob");
    Order buy_low("ABC", 50.00, 2, BUY, bob);
    Order buy_high("ABC", 55.00, 2, BUY, bob);

    // Orders to buy with higher prices are more aggresive than orders
    //   with lower prices and should sort larger as a result.
    ASSERT_LT(buy_high, buy_low);

    Order sell_low("ABC", 50.00, 2, SELL, bob);
    Order sell_high("ABC", 55.00, 2, SELL, bob);

    // Orders to sell with lower prices are more aggresive than orders
    //   with higher prices and should sort larger as a result.
    ASSERT_LT(sell_low, sell_high);
}

TEST(OrderTest, time_order_respected) {
    Client bob("bob");
    Order buy_early("ABC", 50.00, 2, BUY, bob);
    Order buy_later("ABC", 50.00, 2, BUY, bob);

    // Early orders are more aggresive than late orders and should sort
    //   larger as a result.
    ASSERT_LT(buy_early, buy_later);

    Order sell_early("ABC", 50.00, 2, SELL, bob);
    Order sell_later("ABC", 50.00, 2, SELL, bob);

    ASSERT_LT(sell_early, sell_later);
}

TEST(OrderTest, can_cancel_order) {
    Client bob("bob");
    Order o("ABC", 100.00, 2, BUY, bob);
    o.cancel();

    ASSERT_EQ(CANCELLED, o.get_status());
    ASSERT_TRUE(o.is_cancelled());
}

TEST(OrderTest, can_change_filled_amount) {
    Client bob("bob");
    Order o("ABC", 100.00, 10, BUY, bob);

    ASSERT_EQ(10, o.effective_size());

    ASSERT_TRUE(o.fill(5));
    ASSERT_EQ(5, o.effective_size());

    ASSERT_TRUE(o.fill(2));
    ASSERT_EQ(3, o.effective_size());

    ASSERT_TRUE(o.fill(3));
    ASSERT_EQ(0, o.effective_size());
}

TEST(OrderTest, cant_fill_past_order_size) {
    Client bob("bob");
    Order o("ABC", 100.00, 10, BUY, bob);

    ASSERT_FALSE(o.fill(11));
}

TEST(OrderTest, cant_fill_negative_amount) {
    Client bob("bob");
    Order o("ABC", 100.00, 10, BUY, bob);

    ASSERT_FALSE(o.fill(-2));
}

TEST(OrderTest, shows_correct_instrument) {
    Client bob("bob");

    Order o1("ABC", 100.00, 10, BUY, bob);
    ASSERT_STREQ("ABC", o1.get_instrument().c_str());

    Order o2("CBA", 100.00, 10, BUY, bob);
    ASSERT_STREQ("CBA", o2.get_instrument().c_str());
}

TEST(OrderTest, order_is_buy_is_sell) {
    Client bob("bob");

    Order o1("ABC", 100.00, 10, BUY, bob);
    ASSERT_TRUE(o1.is_buy());

    Order o2("CBA", 100.00, 10, SELL, bob);
    ASSERT_TRUE(o2.is_sell());
}

TEST(OrderTest, can_get_price) {
    Client bob("bob");

    Order o1("ABC", 100.00, 10, BUY, bob);
    ASSERT_EQ(100.00, o1.get_price());

    Order o2("ABC", 19.00, 10, SELL, bob);
    ASSERT_EQ(19.00, o2.get_price());
}

TEST(OrderTest, can_sort_orders) {
    Client bob("bob");

    Order o1("ABC", 120.00, 10, BUY, bob);
    Order o2("ABC", 110.00, 10, BUY, bob);
    Order o3("ABC", 100.00, 10, BUY, bob);
    Order o4("ABC", 130.00, 10, BUY, bob);

    std::vector<Order*> buys;

    buys.push_back(&o1);
    buys.push_back(&o2);
    buys.push_back(&o3);
    buys.push_back(&o4);

    std::sort(buys.begin(), buys.end(), [](auto a, auto b) { return *a < *b; });

    ASSERT_EQ(&o4, buys[0]);
    ASSERT_EQ(&o1, buys[1]);
    ASSERT_EQ(&o2, buys[2]);
    ASSERT_EQ(&o3, buys[3]);
}
