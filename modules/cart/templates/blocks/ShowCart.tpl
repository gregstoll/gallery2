{*
 * $Revision: 17145 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="cart.LoadCart"}
<div class="{$class}">
  <h3> {g->text text="Your Cart"} </h3>
  <p>
    {g->text one="You have %d item in your cart" many="You have %d items in your cart"
	     count=$block.cart.ShowCart.total arg1=$block.cart.ShowCart.total}
  </p>
  <a class="{g->linkId view="cart.ViewCart"}" href="{$block.cart.ShowCart.url}">{g->text text="View Cart"}</a>
</div>
