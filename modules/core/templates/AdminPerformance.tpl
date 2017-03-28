{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Performance Tuning"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock">
  <h3 class="giSuccess">
    {if isset($status.saved)}
      {g->text text="Updated performance settings successfully"}
    {else if isset($status.cleared)}
      {g->text text="Deleted all saved pages"}
    {/if}
  </h3>
</div>
{/if}

<div class="gbBlock">
  <h3 class="giTitle">
    {g->text text="Acceleration"}
  </h3>

  <p class="giDescription">
    {g->text text="Improve your Gallery performance by storing entire web pages in the database.  This can considerably reduce the amount of webserver and database resources required to display a web page.  The tradeoff is that the web page you see may be a little bit out of date, however you can always get the most recent version of the page by forcing a refresh in your browser (typically by holding down the shift key and clicking the reload button)."}
  </p>

  <dl class="giDescription">
    <dt style="font-weight: bold"> {g->text text="Partial Acceleration"} </dt>
    <dd>
      <p>
	{g->text text="Partial acceleration gives you roughly 10-25% performance increase, but some forms of dynamic data (like view counts) will not get updated right away.  All content that appears in blocks (like the random image block, any sidebar blocks, etc) will always be updated." cFormat=false}
      </p>
    </dd>
    <dt style="font-weight: bold"> {g->text text="Full Acceleration"} </dt>
    <dd>
      <p>
	{g->text text="Full acceleration gives roughly a 90% performance increase, but no dynamic data (random image block, other sidebar blocks, number of items in your shopping cart, view counts, etc) will get updated until the saved page expires." cFormat=false}
      </p>
    </dd>
  </dl>

  <p class="giDescription">
    {g->text text="You can additionally specify when saved pages expire.  Setting a longer expiration time will reduce the load on your server, but will increase the interval before users see changes.  Lower expiration times mean that users will see more current data, but they will place a higher load on your server."}
  </p>

  <p class="giDescription">
    {g->text text="Here are some standard acceleration profiles:"}
    <br/>
    <a href="javascript:setNoAcceleration()">{g->text text="No acceleration"}</a> &mdash;
    <a href="javascript:setLowTraffic()">{g->text text="Medium acceleration"}</a> &mdash;
    <a href="javascript:setHighTraffic()">{g->text text="High acceleration"}</a>
  </p>

  <table class="gbDataTable">
    <tr>
      <td>
	<b> {g->text text="Guest Users"} </b>
      </td>
      <td>
	<select id="guestType" name="{g->formVar var="form[acceleration][guest][type]"}" onchange="toggleEnabled()">
	  {html_options options=$AdminPerformance.typeList selected=$form.acceleration.guest.type}
	</select>
      </td>
      <td>
	<b>{g->text text="Expires after:"}</b>
	<select id="guestExpire" name="{g->formVar var="form[acceleration][guest][expiration]"}">
	  {html_options options=$AdminPerformance.expirationTimeList selected=$form.acceleration.guest.expiration}
	</select>
      </td>
    </tr>
    <tr class="gbOdd">
      <td>
	<b> {g->text text="Registered Users"} </b>
      </td>
      <td>
	<select id="userType" name="{g->formVar var="form[acceleration][user][type]"}" onchange="toggleEnabled()">
	  {html_options options=$AdminPerformance.typeList selected=$form.acceleration.user.type}
	</select>
      </td>
      <td>
	<b>{g->text text="Expires after:"}</b>
	<select id="userExpire" name="{g->formVar var="form[acceleration][user][expiration]"}">
	  {html_options options=$AdminPerformance.expirationTimeList selected=$form.acceleration.user.expiration}
	</select>
      </td>
    </tr>
  </table>
</div>

<div class="gbBlock">
  <h3 class="giTitle">
    {g->text text="Template Cache"}
  </h3>

  <p class="giDescription">
    {g->text text="For optimal performance, Gallery caches all templates.  If you would like to %scustomize your template files%s, you should disable template caching temporarily so that changes take effect immediately without clearing the template cache." arg1="<a href=\"http://codex.gallery2.org/Gallery2:Editing_Templates\">" arg2="</a>"}
  </p>

  <table class="gbDataTable">
    <tr>
      <td>
        {g->text text="Enable template caching"}
      </td>
      <td>
        <input type="checkbox" {if $form.disableCompileCheck}checked="checked" {/if}
               name="{g->formVar var="form[disableCompileCheck]"}"/>
      </td>
    </tr>
  </table>
</div>


<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][clear]"}" value="{g->text text="Clear Saved Pages"}"/>
</div>

<script type="text/javascript">
var guestType = document.getElementById("guestType");
var guestExpire = document.getElementById("guestExpire");
var userType = document.getElementById("userType");
var userExpire = document.getElementById("userExpire");
{literal}
function setNoAcceleration() {
  guestType.value="none";
  userType.value="none";
  toggleEnabled();
}
function setLowTraffic() {
  guestType.value="partial";
  guestExpire.value="21600";
  userType.value="partial";
  userExpire.value="21600";
  toggleEnabled();
}
function setHighTraffic() {
  guestType.value="full";
  guestExpire.value="86400";
  userType.value="full";
  userExpire.value="86400";
  toggleEnabled();
}
function toggleEnabled() {
  guestExpire.disabled = (guestType.value == "none");
  userExpire.disabled = (userType.value == "none");
}
toggleEnabled();
{/literal}
</script>
