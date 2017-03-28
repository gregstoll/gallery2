{*
 * $Revision: 17679 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{literal}
<script type='text/javascript'>
  function submitBasicMode(){
    document.getElementById('formDigibugCustomerId').value = '';
    document.getElementById('formDigibugEventId').value = '';
    document.getElementById('digibug-mode').value = 'gallery';
    document.getElementById('siteAdminForm').submit();
  }
  function submitAdvancedMode(){
    re = /^[\d]{1,6}$/;
    if (!document.getElementById('formDigibugCustomerId').value.match(re)) {
      document.getElementById('formDigibugCustomerId').style.border = '1px solid red';
      alert('You must enter a valid digibug customer id');
      return;
    } else {
      document.getElementById('formDigibugCustomerId').style.border = '1px solid black';
    }
    if (!document.getElementById('formDigibugEventId').value.match(re)) {
      document.getElementById('formDigibugEventId').style.border = '1px solid red';
      alert('You must enter a valid digibug event id.');
      return;
    } else {
      document.getElementById('formDigibugEventId').style.border = '1px solid black';
    }
    document.getElementById('digibug-mode').value = 'owner';
    document.getElementById('siteAdminForm').submit();
  }
</script>
{/literal}

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Digibug Photo Printing Settings"} </h2>
</div>
{if isset($status.saved)}
<div class="gbBlock">
  <h2 class="giSuccess">
    {g->text text="Settings saved successfully"}
  </h2>
</div>
{/if}
<div class="dig-logo">
</div>
<div class="gbBlock">
  <div class="dig-intro">
    <p class="dig-text">
      {g->text text="Digibug offers you two options for turning your photos into a wide variety of prints, gifts and games. Choose your solution and get started today!"}
  </div>

  <div class="dig-modes">
    <div>
      <input id="digibug-mode" type="hidden" value="gallery" name="{g->formVar var="form[digibugIdChoice]"}"/>
      <input id="digibug-submit" type="hidden" value="Save" name="{g->formVar var="form[action][save]"}"/>
    </div>
    <div class="dig-basic-mode">
      <div class="dig-mode-top"></div>
      <div class="dig-mode-body">
        <div class="mode-title">
  	{g->text text="Digibug Basic"}
	<br/>
	{g->text text="Fast Easy Photo Fulfillment"}
        </div>
        <div class="mode-text">
  	{g->text text="Power up your Gallery with professional level fulfillment from Kodak. Just use Digibug Basic and there's nothing else to do - no registration, no administration, no hassles."}
        </div>
        <div class="mode-items">
	  <ul class="mode-item">
	    <li>{g->text text="Matte and Glossy prints, from 4x6 to as big as 30x40"}</li>
	    <li>{g->text text="Great photo gifts like canvases, apparel, bags, puzzles, mugs and sports memorabilia and more"}</li>
	    <li>{g->text text="Outstanding quality and customer service"}</li>
	  </ul>
        </div>

  	{if $form.digibugIdChoice != 'gallery'}
        <div class="dig-small-rounded clickable" onclick="submitBasicMode();">
	  <br/>
	  {g->text text="CLICK HERE to switch back to Basic mode."}
	</div>
	{else}
	<div class="dig-small-rounded">
	  <br/>
	  {g->text text="You are currently using Basic mode!"}
        </div>
	{/if}

      </div>
      <div class="dig-mode-bottom"></div>
    </div>
    <div class="dig-advanced-mode">
      <div class="dig-mode-top"></div>
      <div class="dig-mode-body">
        <div class="mode-title">
  	{g->text text="Digibug ADVANCED"}
	<br/>
	{g->text text="The Pro's Solution"}
        </div>
        <div class="mode-text">
  	{g->text text="Digibug ADVANCED allows you to set your own price for photos and gifts. Simply provide us with your account information and we'll send you a check each month with your profits. It's the perfect online retail business solution for a photographer - no inventory, no overhead... just profits!"}
        </div>
        <div class="mode-text">
  	{g->text text="Enjoy the same range of professional level photo prints and gifts, but set your own price and charge what you believe your photos are worth. We'll take care of the rest."}
        </div>
        <div class="mode-text dig-sign-in" style="width: 120px;">
  	{g->text text="New to Digibug ADVANCED?"}
	<br/> <br/>
        {g->text text="%sSign up%s to get started"
  	arg1="<a style=\"color:black;font-size:16px;font-weight:bold;text-decoration:underline;\" href=\"http://www.digibug.com/signup.php\">"
	arg2="</a>"}
        </div>
        <div class="advanced-mode-form">
	  <div class="mode-text">{g->text text="Do you have a Digibug Company ID and Event ID?"}</div>
	  <div class="dig-company">
	    <div class="dig-label">{g->text text="Company ID:"}</div>
	    <div class="dig-element">
	      <input type="text" size="6" id="formDigibugCustomerId"
		name="{g->formVar var="form[digibugCustomerId]"}"
		value="{if $form.digibugIdChoice != 'gallery'}{$form.digibugCustomerId}{/if}"/>
	    </div>
	  </div>
	  <div class="dig-event" style="padding-bottom: 20px">
	    <div class="dig-label">{g->text text="Event ID:"}</div>
	    <div class="dig-element">
	      <input type="text" size="6" id="formDigibugEventId"
		name="{g->formVar var="form[digibugPricelistId]"}"
		value="{if $form.digibugIdChoice != 'gallery'}{$form.digibugPricelistId}{/if}"/>
	    </div>
  	  </div>
          <div onclick="submitAdvancedMode();" class="dig-save">{g->text text="SAVE"}</div>
        </div>
      </div>
      <div class="dig-mode-bottom"></div>
    </div>
  </div>
</div>
