{*
 * $Revision: 17148 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Items Currently in Your Cart"} </h2>
</div>

{if !empty($ViewCart.items)}
<form action="{g->url}" method="post">
  <div>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="{$ViewCart.controller}"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="itemId"}" value="{$itemId}"/>
  </div>
  <div class="gbBlock gcBackground2" style="text-align: right; white-space: nowrap">
    <select name="{g->formVar var="form[pluginId]"}">
      <option label="{g->text text="&laquo; cart actions &raquo;"}"
       value="" selected="selected">{g->text text="&laquo; cart actions &raquo;"}</option>
      <option label="{g->text text="Update Quantities"}"
       value="updateCart"> {g->text text="Update Quantities"} </option>
      <option label="{g->text text="Empty Cart"}"
       value="emptyCart"> {g->text text="Empty Cart"} </option>

      {foreach from=$ViewCart.plugins key=pluginId item=pluginData}
      {if $pluginData.isAvailable}
      <option label="{$pluginData.actionDisplayName}"
       value="{$pluginId}"> {$pluginData.actionDisplayName} </option>
      {/if}
      {/foreach}
    </select>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][modify]"}" value="{g->text text="Go"}"/>
  </div>
{/if}

{if isset($status.cartModified)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Cart updated successfully"}
</h2></div>
{/if}

{if empty($ViewCart.items)}
<div class="gbBlock"><strong>
  <p> {g->text text="Your cart is empty."} </p>
  <p>
    {g->text text="To add items, browse the gallery and select 'Add to cart' from the item's action menu."}
  </p>
  </strong>
</div>
{else}

<div class="gbBlock">
  <table class="gbDataTable"><tr>
    <th> {g->text text="Quantity"} </th>
    <th colspan="2" align="center"> {g->text text="Item"} </th>
    <th> {g->text text="Type"} </th>
    <th> {g->text text="Remove"} </th>
  </tr>
  {foreach from=$ViewCart.items item=item}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      <input type="text" size="3"
       name="{g->formVar var="form[counts][`$item.id`]"}" value="{$form.counts[$item.id]}"/>
    </td><td>
      <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$item.id`"}">
        {if isset($ViewCart.thumbnails[$item.id])}
  	{g->image item=$ViewCart.items[$item.id] image=$ViewCart.thumbnails[$item.id]
  		  maxSize=90}
        {else}
  	{g->text text="No thumbnail"}
        {/if}
      </a>
    </td><td>
      <ul>
        <li>
  	<strong>{g->text text="Title:"}</strong>
  	 {$item.title|markup}
        </li>
        <li>
  	<strong>{g->text text="Summary:"}</strong>
  	{$item.summary|markup}
        </li>
      </ul>
    </td><td>
      {$ViewCart.itemTypeNames[$item.id].0}
    </td><td align="center">
      <input type="checkbox" name="{g->formVar var="form[delete][`$item.id`]"}"/>
    </td>
  </tr>
  {/foreach}
  </table>
</div>
</form>
{/if}

