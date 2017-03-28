{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Registration Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {/if}
  {if isset($status.activated)}
    {g->text text="Activated user %s" arg1=$status.activated}
  {/if}
  {if isset($status.deleted)}
    {g->text text="Deleted user %s" arg1=$status.deleted}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Confirmation Policy"} </h3>

  <p class="giDescription">
   {g->text text="The Registration module can accept new user registrations instantly, require the user to click a confirmation link in an email that is sent by the module, or require account activation by a site administrator."}
  </p>

  {g->text text="Choose policy:"}
  <select name="{g->formVar var="form[confirmation]"}">
    {html_options options=$SelfRegistration.emailConfirmationList selected=$form.confirmation}
  </select>
</div>

<div class="gbBlock">
  <h3> {g->text text="Email details"} </h3>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Sender(From) Email Address:"}
    </td><td>
      <input type="text"  size="30"
       name="{g->formVar var="form[from]"}" value="{$form.from}"/>
    </td>
  </tr><tr>
    <td>
      {g->text text="Confirmation Email Subject:"}
    </td><td>
      <input type="text" size="30"
       name="{g->formVar var="form[subject]"}" value="{$form.subject}"/>
    </td>
  </tr><tr>
    <td colspan="2">
      <input type="checkbox" id="cbEmailAdmins" {if $form.emailadmins}checked="checked" {/if}
       name="{g->formVar var="form[emailadmins]"}"/>
      <label for="cbEmailAdmins">
	{g->text text="Email Site Administrators for all new registrations"}
      </label>
    </td>
  </tr><tr>
    <td>
      {g->text text="Admin Email Subject:"}
    </td><td>
      <input type="text" size="30"
       name="{g->formVar var="form[adminsubject]"}" value="{$form.adminsubject}"/>
    </td>
  </tr><tr>
    <td colspan="2">
      <input type="checkbox" id="cbEmailUsers" {if $form.emailusers}checked="checked" {/if}
       name="{g->formVar var="form[emailusers]"}"/>
      <label for="cbEmailUsers">
	{g->text text="Email new users upon account activation"}
      </label>
    </td>
  </tr><tr>
    <td>
      {g->text text="Welcome Email Subject:"}
    </td><td>
      <input type="text" size="30"
       name="{g->formVar var="form[usersubject]"}" value="{$form.usersubject}"/>
    </td>
  </tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Reset"}"/>
</div>

{if $form.list.count>0}
<div class="gbBlock">
  <h3> {g->text text="Pending Registrations"} </h3>

  {if ($form.list.maxPages > 1)}
    <div style="margin-bottom: 10px"><span class="gcBackground1" style="padding: 5px">
      <input type="hidden"
       name="{g->formVar var="form[list][page]"}" value="{$form.list.page}"/>
      <input type="hidden"
       name="{g->formVar var="form[list][maxPages]"}" value="{$form.list.maxPages}"/>

      {if ($form.list.page > 1)}
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=register.AdminSelfRegistration"
	 arg3="form[list][page]=1"}">{g->text text="&laquo; first"}</a>
	&nbsp;
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=register.AdminSelfRegistration"
	 arg3="form[list][page]=`$form.list.backPage`"}">{g->text text="&laquo; back"}</a>
      {/if}

      &nbsp;
      {g->text text="Viewing page %d of %d" arg1=$form.list.page arg2=$form.list.maxPages}
      &nbsp;

      {if ($form.list.page < $form.list.maxPages)}
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=register.AdminSelfRegistration"
	 arg3="form[list][page]=`$form.list.nextPage`"}">{g->text text="next &raquo;"}</a>
	&nbsp;
	<a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=register.AdminSelfRegistration"
	 arg3="form[list][page]=`$form.list.maxPages`"}">{g->text text="last &raquo;"}</a>
      {/if}
    </span></div>
  {/if}

  <table class="gbDataTable"><tr>
    <th> {g->text text="Username"} </th>
    <th> {g->text text="Full Name"} </th>
    <th> {g->text text="Email"} </th>
    <th> {g->text text="Date"} </th>
    <th> {g->text text="Action"} </th>
  </tr>

  {foreach from=$form.list.userNames key=userId item=user}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td> {$user.userName} </td>
    <td> {$user.fullName} </td>
    <td> {$user.email} </td>
    <td> {g->date timestamp=$user.creationTimestamp} </td>
    <td>
      <a href="{g->url arg1="controller=register.AdminSelfRegistration"
       arg2="form[action][activate]=1" arg3="form[userId]=$userId"}">{g->text text="activate"}</a>
      &nbsp;
      <a href="{g->url arg1="controller=register.AdminSelfRegistration"
       arg2="form[action][delete]=1" arg3="form[userId]=$userId"}">{g->text text="delete"}</a>
    </td>
  </tr>
  {/foreach}
</table>
</div>
{/if}
